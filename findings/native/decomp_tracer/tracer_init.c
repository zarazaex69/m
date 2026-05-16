
// WARNING: Variable defined which should be unmapped: var_10h
// WARNING: Variable defined which should be unmapped: var_20h
// WARNING: Variable defined which should be unmapped: var_30h
// WARNING: Variable defined which should be unmapped: var_40h
// WARNING: Variable defined which should be unmapped: var_50h

void sym.tracer_init(int64_t arg1, int64_t arg2)
{
    unsigned long long arg1_00;
    int64_t iVar1;
    uint64_t uVar2;
    int64_t iVar3;
    int64_t arg2_00;
    int64_t in_x2;
    char *arg3;
    int64_t in_x3;
    int64_t in_x4;
    int64_t in_x5;
    int64_t in_x6;
    int64_t in_x7;
    undefined8 *puVar4;
    uint64_t uVar5;
    int64_t var_98h;
    undefined8 uStack_90;
    int64_t var_88h;
    char *var_80h;
    uint64_t uStack_78;
    int64_t var_70h;
    int64_t iStack_68;
    char cStack_60;
    int64_t iStack_58;
    int64_t var_50h;
    char *var_40h;
    int64_t var_30h;
    int64_t var_20h;
    int64_t var_10h;
    
    iVar3 = *(int64_t *)0xc57b0;
    iVar1 = tpidr_el0;
    iStack_58 = *(int64_t *)(iVar1 + 0x28);
    iStack_68 = *(int64_t *)0xc57b0;
    cStack_60 = '\x01';
    fcn.00073d24(*(int64_t *)0xc57b0);
    puVar4 = *(undefined8 **)(iVar3 + 0x90);
    if (puVar4 == (undefined8 *)0x0) {
code_r0x0004ef68:
        fcn.00073dbc(iVar3);
    } else {
        uVar2 = sym.imp.strlen(arg1);
        if (0xffffffffffffffef < uVar2) {
            if (*(int64_t *)(iVar1 + 0x28) == iStack_58) {
                iVar3 = fcn.0004931c(&var_80h);
                if ((var_98h & 1U) != 0) {
                    fcn.000b3554(var_88h);
                }
                if (((uint64_t)var_80h & 1) != 0) {
                    fcn.000b3554(var_70h);
                }
                fcn.00050738((int64_t)&iStack_68);
                if (*(int64_t *)(iVar1 + 0x28) == iStack_58) {
                    fcn.000b576c(iVar3, arg2_00, in_x2, in_x3, in_x4, in_x5, in_x6, in_x7);
                }
            }
            goto code_r0x0004f008;
        }
        if (uVar2 < 0x17) {
            uVar5 = (uint64_t)&var_80h | 1;
            var_80h = (char *)CONCAT71(var_80h._1_7_, (char)((int32_t)uVar2 << 1));
            if (uVar2 != 0) goto code_r0x0004eedc;
        } else {
            arg1_00 = (uVar2 | 0xf) + 1;
            uVar5 = fcn.000b34a4(arg1_00);
            var_80h = (char *)(arg1_00 | 1);
            uStack_78 = uVar2;
            var_70h = uVar5;
code_r0x0004eedc:
            sym.imp.memcpy(uVar5, arg1, uVar2);
        }
        arg3 = (char *)(arg2 + -1);
        *(undefined *)(uVar5 + uVar2) = 0;
        var_98h = 0;
        uStack_90 = 0;
        var_88h = 0;
        do {
            arg3 = arg3 + 1;
        } while (*arg3 != '\0');
        fcn.0004e010((int64_t)&var_98h, arg2, (unsigned long long)arg3);
        (**(code **)*puVar4)(puVar4, &var_80h, &var_98h);
        if ((var_98h & 1U) != 0) {
            fcn.000b3554(var_88h);
        }
        iVar3 = iStack_68;
        if (((uint64_t)var_80h & 1) != 0) {
            fcn.000b3554(var_70h);
            iVar3 = iStack_68;
        }
        iStack_68 = iVar3;
        if (cStack_60 != '\0') goto code_r0x0004ef68;
    }
    if (*(int64_t *)(iVar1 + 0x28) == iStack_58) {
        return;
    }
code_r0x0004f008:
    // WARNING: Subroutine does not return
    sym.imp.__stack_chk_fail();
}
