
// WARNING: Variable defined which should be unmapped: var_10h
// WARNING: Variable defined which should be unmapped: var_20h
// WARNING: Variable defined which should be unmapped: var_30h

void sym.tracer_set_ssl_cainfo(int64_t arg1, int64_t arg2)
{
    int64_t iVar1;
    int64_t iVar2;
    char *pcVar3;
    int64_t *piVar4;
    int64_t var_78h;
    undefined8 uStack_70;
    int64_t var_68h;
    char *var_60h;
    undefined8 uStack_58;
    int64_t var_50h;
    int64_t iStack_48;
    undefined uStack_40;
    int64_t iStack_38;
    int64_t var_30h;
    int64_t var_20h;
    int64_t var_10h;
    
    iVar2 = *(int64_t *)0xc57b0;
    iVar1 = tpidr_el0;
    iStack_38 = *(int64_t *)(iVar1 + 0x28);
    iStack_48 = *(int64_t *)0xc57b0;
    uStack_40 = 1;
    fcn.00073d24(*(int64_t *)0xc57b0);
    piVar4 = *(int64_t **)(iVar2 + 0x90);
    if (piVar4 != (int64_t *)0x0) {
        pcVar3 = (char *)(arg1 + -1);
        var_60h = (char *)0x0;
        uStack_58 = 0;
        var_50h = 0;
        do {
            pcVar3 = pcVar3 + 1;
        } while (*pcVar3 != '\0');
        fcn.0004e010((int64_t)&var_60h, arg1, (unsigned long long)pcVar3);
        pcVar3 = (char *)(arg2 + -1);
        var_78h = 0;
        uStack_70 = 0;
        var_68h = 0;
        do {
            pcVar3 = pcVar3 + 1;
        } while (*pcVar3 != '\0');
        fcn.0004e010((int64_t)&var_78h, arg2, (unsigned long long)pcVar3);
        (**(code **)(*piVar4 + 0x48))(piVar4, &var_60h, &var_78h);
        if ((var_78h & 1U) != 0) {
            fcn.000b3554(var_68h);
        }
        if (((uint64_t)var_60h & 1) != 0) {
            fcn.000b3554(var_50h);
        }
    }
    fcn.00073dbc(0xca868);
    if (*(int64_t *)(iVar1 + 0x28) == iStack_38) {
        return;
    }
    // WARNING: Subroutine does not return
    sym.imp.__stack_chk_fail();
}
