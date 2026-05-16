
// WARNING: Variable defined which should be unmapped: var_10h
// WARNING: Variable defined which should be unmapped: var_20h
// WARNING: Variable defined which should be unmapped: var_30h
// WARNING: Variable defined which should be unmapped: var_40h

void sym.tracer_set_userid(int64_t arg1)
{
    unsigned long long arg1_00;
    int64_t iVar1;
    uint64_t uVar2;
    int64_t arg2;
    int64_t in_x2;
    int64_t in_x3;
    int64_t in_x4;
    int64_t in_x5;
    int64_t in_x6;
    int64_t in_x7;
    int64_t iVar3;
    int64_t *piVar4;
    uint64_t uVar5;
    int64_t var_70h;
    uint64_t uStack_68;
    int64_t var_60h;
    int64_t var_58h;
    char *var_50h;
    int64_t iStack_48;
    int64_t var_40h;
    int64_t var_30h;
    int64_t var_20h;
    int64_t var_10h;
    
    iVar3 = *(int64_t *)0xc57b0;
    iVar1 = tpidr_el0;
    iStack_48 = *(int64_t *)(iVar1 + 0x28);
    var_58h = *(int64_t *)0xc57b0;
    var_50h._0_1_ = '\x01';
    fcn.00073d24(*(int64_t *)0xc57b0);
    piVar4 = *(int64_t **)(iVar3 + 0x90);
    if (piVar4 == (int64_t *)0x0) {
code_r0x0004f990:
        fcn.00073dbc(iVar3);
    } else {
        uVar2 = sym.imp.strlen(arg1);
        if (0xffffffffffffffef < uVar2) {
            if (*(int64_t *)(iVar1 + 0x28) == iStack_48) {
                iVar3 = fcn.0004931c(&var_70h);
                if ((var_70h & 1U) != 0) {
                    fcn.000b3554(var_60h);
                }
                fcn.00050738((int64_t)&var_58h);
                if (*(int64_t *)(iVar1 + 0x28) == iStack_48) {
                    fcn.000b576c(iVar3, arg2, in_x2, in_x3, in_x4, in_x5, in_x6, in_x7);
                }
            }
            goto code_r0x0004fa18;
        }
        if (uVar2 < 0x17) {
            uVar5 = (uint64_t)&var_70h | 1;
            var_70h = CONCAT71(var_70h._1_7_, (char)((int32_t)uVar2 << 1));
            if (uVar2 != 0) goto code_r0x0004f94c;
        } else {
            arg1_00 = (uVar2 | 0xf) + 1;
            uVar5 = fcn.000b34a4(arg1_00);
            var_70h = arg1_00 | 1;
            uStack_68 = uVar2;
            var_60h = uVar5;
code_r0x0004f94c:
            sym.imp.memcpy(uVar5, arg1, uVar2);
        }
        iVar3 = *piVar4;
        *(undefined *)(uVar5 + uVar2) = 0;
        (**(code **)(iVar3 + 0x18))(piVar4, &var_70h);
        if ((var_70h & 1U) != 0) {
            fcn.000b3554(var_60h);
        }
        iVar3 = var_58h;
        if ((char)var_50h != '\0') goto code_r0x0004f990;
    }
    if (*(int64_t *)(iVar1 + 0x28) == iStack_48) {
        return;
    }
code_r0x0004fa18:
    // WARNING: Subroutine does not return
    sym.imp.__stack_chk_fail();
}
