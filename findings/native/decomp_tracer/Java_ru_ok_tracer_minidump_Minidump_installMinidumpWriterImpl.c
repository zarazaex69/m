
// WARNING: Variable defined which should be unmapped: var_10h
// WARNING: Variable defined which should be unmapped: var_20h
// WARNING: Variable defined which should be unmapped: var_30h
// WARNING: Variable defined which should be unmapped: var_40h

void sym.Java_ru_ok_tracer_minidump_Minidump_installMinidumpWriterImpl
               (int64_t arg1, undefined8 placeholder_1, int64_t arg3)
{
    int64_t arg1_00;
    int64_t iVar1;
    int64_t iVar2;
    undefined8 uVar3;
    unsigned long long arg2;
    char *arg3_00;
    undefined8 *puVar4;
    int64_t var_70h;
    undefined8 uStack_68;
    int64_t var_60h;
    int64_t iStack_58;
    undefined uStack_50;
    int64_t iStack_48;
    int64_t var_40h;
    int64_t var_30h;
    int64_t var_20h;
    int64_t var_10h;
    
    iVar1 = tpidr_el0;
    iStack_48 = *(int64_t *)(iVar1 + 0x28);
    uVar3 = (**(code **)(*(int64_t *)arg1 + 0x548))(arg1, arg3, 0);
    arg2 = sym.imp.strdup();
    (**(code **)(*(int64_t *)arg1 + 0x550))(arg1, arg3, uVar3);
    iVar2 = *(int64_t *)0xc57b0;
    uStack_50 = 1;
    arg1_00 = *(int64_t *)0xc57b0 + 0x98;
    iStack_58 = arg1_00;
    fcn.00073d24(arg1_00);
    puVar4 = *(undefined8 **)(iVar2 + 0x128);
    if (puVar4 != (undefined8 *)0x0) {
        arg3_00 = (char *)(arg2 - 1);
        var_70h = 0;
        uStack_68 = 0;
        var_60h = 0;
        do {
            arg3_00 = arg3_00 + 1;
        } while (*arg3_00 != '\0');
        fcn.0004e010((int64_t)&var_70h, arg2, (unsigned long long)arg3_00);
        (**(code **)*puVar4)(puVar4, &var_70h);
        if ((var_70h & 1U) != 0) {
            fcn.000b3554(var_60h);
        }
    }
    fcn.00073dbc(arg1_00);
    if (*(int64_t *)(iVar1 + 0x28) == iStack_48) {
        return;
    }
    // WARNING: Subroutine does not return
    sym.imp.__stack_chk_fail();
}
