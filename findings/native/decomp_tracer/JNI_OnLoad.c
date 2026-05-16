
// WARNING: Variable defined which should be unmapped: var_10h
// WARNING: Variable defined which should be unmapped: var_20h
// WARNING: Variable defined which should be unmapped: var_30h

undefined8 sym.JNI_OnLoad(int64_t arg1)
{
    int64_t iVar1;
    undefined8 *puVar2;
    undefined8 *puVar3;
    undefined8 uVar4;
    undefined8 uVar5;
    int64_t var_40h;
    int64_t var_38h;
    int64_t var_30h;
    char *var_20h;
    int64_t var_10h;
    
    iVar1 = tpidr_el0;
    var_38h = *(int64_t *)(iVar1 + 0x28);
    **(int64_t **)0xc57f8 = arg1;
    (**(code **)(*(int64_t *)arg1 + 0x20))(arg1, &var_40h, 0);
    uVar4 = (**(code **)(*(int64_t *)var_40h + 0x30))(var_40h, "ru/ok/tracer/nativebridge/NativeBridge");
    puVar3 = *(undefined8 **)0xc5808;
    **(undefined8 **)0xc5808 = uVar4;
    uVar4 = (**(code **)(*(int64_t *)var_40h + 0x108))
                      (var_40h, uVar4, "setKey", "(Ljava/lang/String;Ljava/lang/String;)V");
    puVar2 = *(undefined8 **)0xc5800;
    uVar5 = *puVar3;
    **(undefined8 **)0xc5800 = uVar4;
    uVar4 = (**(code **)(*(int64_t *)var_40h + 0x108))(var_40h, uVar5, 0x23e79, "(Ljava/lang/String;)V");
    puVar2[1] = uVar4;
    if (*(int64_t *)(iVar1 + 0x28) == var_38h) {
        return 0x10006;
    }
    // WARNING: Subroutine does not return
    sym.imp.__stack_chk_fail();
}
