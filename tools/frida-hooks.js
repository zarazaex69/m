/**
 * Frida Hook Scripts for Max Messenger Dynamic Analysis
 * FOR RESEARCH PURPOSES ONLY — use on your own device.
 *
 * Usage: frida -U -f <package> -l frida-hooks.js --no-pause
 */

'use strict';

Java.perform(function () {
    console.log('[*] Frida hooks loaded for Max messenger analysis');

    // =========================================================================
    // 1) VPN Detection Bypass
    //    NetworkCapabilities.hasTransport(4) => false (transport 4 = VPN)
    // =========================================================================
    var NetworkCapabilities = Java.use('android.net.NetworkCapabilities');
    NetworkCapabilities.hasTransport.implementation = function (transport) {
        if (transport === 4) {
            console.log('[VPN Bypass] hasTransport(VPN) intercepted, returning false');
            return false;
        }
        return this.hasTransport(transport);
    };

    // =========================================================================
    // 2) Certificate Pinning Bypass
    //    TrustManager.checkServerTrusted => no-op
    // =========================================================================
    var X509TrustManager = Java.use('javax.net.ssl.X509TrustManager');
    var TrustManagerImpl = Java.use('com.android.org.conscrypt.TrustManagerImpl');

    TrustManagerImpl.checkServerTrusted.overload('[Ljava.security.cert.X509Certificate;', 'java.lang.String').implementation = function (chain, authType) {
        console.log('[Pin Bypass] checkServerTrusted called, authType=' + authType + ', chain length=' + chain.length);
    };

    TrustManagerImpl.checkServerTrusted.overload('[Ljava.security.cert.X509Certificate;', 'java.lang.String', 'java.lang.String').implementation = function (chain, authType, host) {
        console.log('[Pin Bypass] checkServerTrusted called, host=' + host + ', authType=' + authType);
        return Java.use('java.util.Arrays').asList(chain);
    };

    // =========================================================================
    // 3) API Request Logging — intercept y78 signature computation
    // =========================================================================
    try {
        var Y78 = Java.use('y78');
        var methods = Y78.class.getDeclaredMethods();
        methods.forEach(function (method) {
            var name = method.getName();
            Y78[name].overloads.forEach(function (overload) {
                overload.implementation = function () {
                    var args = Array.prototype.slice.call(arguments);
                    var result = overload.apply(this, arguments);
                    console.log('[API Sig] y78.' + name + '(' + JSON.stringify(args.map(String)) + ') => ' + result);
                    return result;
                };
            });
        });
        console.log('[*] y78 signature hooks installed');
    } catch (e) {
        console.log('[!] y78 class not found: ' + e.message);
    }

    // =========================================================================
    // 4) WebSocket Message Logging — intercept x5d packet parsing
    // =========================================================================
    try {
        var X5D = Java.use('x5d');
        var x5dMethods = X5D.class.getDeclaredMethods();
        x5dMethods.forEach(function (method) {
            var name = method.getName();
            X5D[name].overloads.forEach(function (overload) {
                overload.implementation = function () {
                    var args = Array.prototype.slice.call(arguments);
                    var result = overload.apply(this, arguments);
                    console.log('[WS Packet] x5d.' + name + ' args=' + JSON.stringify(args.map(function (a) {
                        if (a && a.getClass) return a.getClass().getName() + '@' + a.hashCode();
                        return String(a);
                    })));
                    if (result && result.getClass) {
                        console.log('[WS Packet]   => ' + result.getClass().getName() + ': ' + result.toString());
                    }
                    return result;
                };
            });
        });
        console.log('[*] x5d WebSocket hooks installed');
    } catch (e) {
        console.log('[!] x5d class not found: ' + e.message);
    }

    // =========================================================================
    // 5) Deeplink Logging — intercept ucc.m22811u
    // =========================================================================
    try {
        var UCC = Java.use('ucc');
        UCC.m22811u.overloads.forEach(function (overload) {
            overload.implementation = function () {
                var args = Array.prototype.slice.call(arguments);
                console.log('[Deeplink] ucc.m22811u called with: ' + JSON.stringify(args.map(String)));
                var result = overload.apply(this, arguments);
                console.log('[Deeplink]   => ' + result);
                return result;
            };
        });
        console.log('[*] Deeplink hooks installed');
    } catch (e) {
        console.log('[!] ucc class not found: ' + e.message);
    }

    // =========================================================================
    // 6) Push Notification Logging — intercept cei push handler
    // =========================================================================
    try {
        var CEI = Java.use('cei');
        var ceiMethods = CEI.class.getDeclaredMethods();
        ceiMethods.forEach(function (method) {
            var name = method.getName();
            CEI[name].overloads.forEach(function (overload) {
                overload.implementation = function () {
                    var args = Array.prototype.slice.call(arguments);
                    console.log('[Push] cei.' + name + ' args=' + JSON.stringify(args.map(function (a) {
                        if (a === null) return 'null';
                        if (a && a.getClass) {
                            try { return a.toString(); } catch (_) { return a.getClass().getName(); }
                        }
                        return String(a);
                    })));
                    return overload.apply(this, arguments);
                };
            });
        });
        console.log('[*] Push notification hooks installed');
    } catch (e) {
        console.log('[!] cei class not found: ' + e.message);
    }

    // =========================================================================
    // 7) SharedPreferences Monitoring — getString/putString for sensitive keys
    // =========================================================================
    var SharedPreferencesImpl = Java.use('android.app.SharedPreferencesImpl');
    SharedPreferencesImpl.getString.implementation = function (key, defValue) {
        var value = this.getString(key, defValue);
        var sensitive = ['token', 'auth', 'session', 'secret', 'key', 'password', 'cookie', 'refresh'];
        var keyLower = key ? key.toLowerCase() : '';
        if (sensitive.some(function (s) { return keyLower.indexOf(s) !== -1; })) {
            console.log('[SharedPrefs GET] key="' + key + '" value="' + value + '"');
        }
        return value;
    };

    var SharedPreferencesEditor = Java.use('android.app.SharedPreferencesImpl$EditorImpl');
    SharedPreferencesEditor.putString.implementation = function (key, value) {
        var sensitive = ['token', 'auth', 'session', 'secret', 'key', 'password', 'cookie', 'refresh'];
        var keyLower = key ? key.toLowerCase() : '';
        if (sensitive.some(function (s) { return keyLower.indexOf(s) !== -1; })) {
            console.log('[SharedPrefs PUT] key="' + key + '" value="' + value + '"');
        }
        return this.putString(key, value);
    };

    // =========================================================================
    // 8) Native Function Hooking — nativeCropAndScale to log dimensions
    // =========================================================================
    var modules = Process.enumerateModules();
    var targetLib = modules.find(function (m) { return m.name.indexOf('max') !== -1 || m.name.indexOf('native') !== -1; });

    var hookNative = function (moduleName) {
        var exports = Module.enumerateExports(moduleName);
        var cropFn = exports.find(function (e) { return e.name.indexOf('nativeCropAndScale') !== -1; });
        if (cropFn) {
            Interceptor.attach(cropFn.address, {
                onEnter: function (args) {
                    console.log('[Native] nativeCropAndScale called');
                    console.log('[Native]   arg0 (ptr)=' + args[0]);
                    console.log('[Native]   arg1 (srcW)=' + args[1].toInt32());
                    console.log('[Native]   arg2 (srcH)=' + args[2].toInt32());
                    console.log('[Native]   arg3 (dstW)=' + args[3].toInt32());
                    console.log('[Native]   arg4 (dstH)=' + args[4].toInt32());
                },
                onLeave: function (retval) {
                    console.log('[Native]   => returned ' + retval);
                }
            });
            console.log('[*] Native nativeCropAndScale hook installed in ' + moduleName);
            return true;
        }
        return false;
    };

    // Try all loaded modules
    var hooked = false;
    modules.forEach(function (m) {
        if (!hooked) hooked = hookNative(m.name);
    });

    if (!hooked) {
        // Fallback: hook when library loads later
        var dlopen = Module.findExportByName(null, 'android_dlopen_ext') || Module.findExportByName(null, 'dlopen');
        if (dlopen) {
            Interceptor.attach(dlopen, {
                onEnter: function (args) {
                    this.path = args[0].readCString();
                },
                onLeave: function () {
                    if (this.path && !hooked) {
                        hooked = hookNative(this.path);
                    }
                }
            });
        }
        console.log('[*] Native hook deferred — waiting for library load');
    }

    console.log('[*] All hooks initialized successfully');
});
