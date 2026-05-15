# VULN-41: Barcode Scanner & ML Kit Analysis

## Summary

Max messenger uses Google ML Kit exclusively for barcode/QR code scanning. The manifest declares `com.google.mlkit.vision.DEPENDENCIES=barcode`. The scanner serves two primary modes: device login (LOGIN) and webapp QR reading (WEBAPP). The scanned QR text is passed **without URL/scheme validation** to consumers, creating potential attack surface.

## 1. Grep Results (Key References)

```
one/p011me/qrscanner/QrScannerWidget.java - Main QR scanner UI widget
p000/fre.java - QR scanner presenter/ViewModel ("GoogleMlKit analyzer")
p000/lgb.java - MlKitAnalyzer (camera image analysis pipeline)
p000/iu7.java - Barcode result callback handler
p000/sgb.java - MLKit base detector (processes YUV_420_888 images)
p000/gul.java - Barcode scanner client (com.google.mlkit.dynamite.barcode)
p000/zqe.java - QrScannerMode enum: WEBAPP(1), LOGIN(2)
p000/hg0.java - Auth QR handler (sends qrLink to server)
p000/tak.java - WebApp QR code reader delegate (WebAppOpenCodeReader)
```

## 2. QR Scanner Uses Beyond Login

| Mode | Purpose | Handler |
|------|---------|---------|
| LOGIN | Device authentication via QR code | `SettingsDevicesScreen` → `eyg` → `hg0.m9501a()` sends `qrLink` param to server |
| WEBAPP | Mini-app/webapp QR code reading | `WebAppRootScreen` → `ijk.f27794v1` → `iak.m26324a()` passes raw text back to webapp JS |

The webapp mode (`WebAppOpenCodeReader`) allows mini-apps to request QR scanning and receive the raw scanned text. This is triggered via JsBridge from webapp JavaScript.

## 3. Automatic Actions from Scanned QR Codes

**Yes, automatic actions occur:**

- **Gallery scan path** (`mo13174y0`): When QR is scanned from a gallery image, `Uri.parse(str)` is called directly on the scanned text (line 791 of QrScannerWidget), then passed to `m16785g1()` which triggers the local image analysis pipeline.
- **Camera scan path** (`m16786h1`): After QR detection + animation, the raw text is wrapped in `h4g(str)` (Success result) and passed back to the caller via `fre.m7839u()` → `bre(PopWithResult)`.
- **LOGIN mode**: The raw QR text is sent directly to the server as `qrLink` parameter via `hg0.m9501a()` — no client-side URL validation observed.
- **WEBAPP mode**: The raw QR text is passed directly back to the webapp JavaScript via `iak.m26324a(text)` — the webapp can then do anything with it.

**No URL/scheme validation** is performed on the scanned QR text before it is consumed.

## 4. Vulnerability Assessment: Crafted QR Code Execution

### Risk: MEDIUM-HIGH

**Attack vectors identified:**

1. **LOGIN mode - Server-side injection**: The scanned QR text is sent as `qrLink` to the server without client-side sanitization. If the server doesn't validate, a crafted QR could potentially:
   - Trigger auth for an attacker-controlled session
   - Inject malicious payloads into the auth flow

2. **WEBAPP mode - JavaScript injection via QR**: A malicious webapp (mini-app) can request QR scanning via `WebAppOpenCodeReader` JsBridge action. The scanned text is returned raw to the webapp's JavaScript. If the webapp processes this unsafely (e.g., `eval()`, DOM injection), a crafted QR could trigger code execution within the webapp context.

3. **Gallery scan - Uri.parse without validation**: `Uri.parse(str)` is called on raw QR text from gallery images. While this alone doesn't trigger navigation, it feeds into the image analysis pipeline which could be confused by malformed URIs.

4. **No scheme filtering**: There is no check for dangerous URI schemes (`javascript:`, `intent:`, `file:`, `content:`) in the QR scan result before it's consumed.

### What prevents full RCE:
- The QR text is not directly opened in a browser or via `ACTION_VIEW` intent
- LOGIN mode sends to server (server-side validation is the defense)
- WEBAPP mode returns to JS (webapp sandbox is the defense)
- No `startActivity(Intent(ACTION_VIEW, Uri.parse(qrText)))` pattern found

## 5. Other ML Models Processing Camera Feed

**Only barcode scanning uses the camera feed.** The ML Kit integration is limited to:
- `com.google.mlkit.dynamite.barcode` - Barcode/QR detection
- `com.google.mlkit.vision.barcode.bundled.internal.ThickBarcodeScannerCreator`

The `wbl.java` and `zol.java` files contain **logging event definitions** for many ML Kit features (face detection, text recognition, object detection, pose detection, segmentation, image labeling, etc.), but these are part of the ML Kit SDK's telemetry infrastructure — **not actual feature implementations**. Only barcode detection is actively used.

## 6. Camera Permission Usage

Camera is requested for:
- **QR Scanner** (`qrscanner_camera_request_description`) - QR/barcode scanning
- **Chat attachment** (`CHAT_ATTACH_PICKER_CAMERA`) - Photo/video capture in chat
- **Avatar picker** (`AVATAR_PICKER_CAMERA`) - Profile photo capture
- **Mini-app camera** (`MINIAPP_CAMERA_PERMISSION`, `MINIAPP_CAMERA`) - Webapp/mini-app camera access
- **Video calls** (via `CameraxCameraApiView`) - Video calling
- **Video messages** - Recording video messages

No unexpected or covert camera usage was found beyond these documented features.

## 7. Face Detection/Recognition Code

**No active face detection/recognition implementation found.** The references to face detection are:
- `onDeviceFaceDetectionLogEvent` - ML Kit telemetry event definition (in `wbl.java`)
- `cloudFaceDetectionLogEvent` - ML Kit telemetry event definition
- `onDeviceFaceLoadLogEvent` - ML Kit telemetry event definition
- `aggregatedOnDeviceFaceDetectionLogEvent` - ML Kit telemetry event definition

These are **standard ML Kit SDK logging schemas** bundled with the library, not active feature code. No `FaceDetector`, `FaceDetectorOptions`, or face processing pipeline was found in the application code.

## Security Recommendations

1. **Add URL scheme validation** on QR scan results before passing to consumers — block `javascript:`, `intent:`, `file:`, `data:` schemes
2. **Rate-limit QR scan attempts** in LOGIN mode to prevent brute-force session hijacking
3. **Add user confirmation dialog** before sending QR auth requests to server (currently auto-sends after scan animation)
4. **Validate webapp origin** before allowing `WebAppOpenCodeReader` JsBridge calls — ensure only trusted mini-apps can request QR scanning
5. **Consider adding a preview** of scanned QR content before acting on it in LOGIN mode
