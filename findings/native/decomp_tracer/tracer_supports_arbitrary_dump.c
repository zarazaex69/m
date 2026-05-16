
// WARNING: Variable defined which should be unmapped: var_10h
// WARNING: Variable defined which should be unmapped: var_20h
// WARNING: Variable defined which should be unmapped: var_30h

uint32_t sym.tracer_supports_arbitrary_dump(void)
{
    int64_t arg1;
    int64_t iVar1;
    int64_t iVar2;
    uint32_t uVar3;
    int64_t iVar4;
    int64_t var_30h;
    int64_t var_20h;
    int64_t var_10h;
    
    iVar2 = *(int64_t *)0xc57b0;
    iVar1 = tpidr_el0;
    iVar4 = *(int64_t *)(iVar1 + 0x28);
    arg1 = *(int64_t *)0xc57b0 + 0x98;
    fcn.00073d24(arg1);
    if (*(int64_t **)(iVar2 + 0x128) == (int64_t *)0x0) {
        uVar3 = 0;
    } else {
        uVar3 = (**(code **)(**(int64_t **)(iVar2 + 0x128) + 0x18))();
    }
    fcn.00073dbc(arg1);
    if (*(int64_t *)(iVar1 + 0x28) == iVar4) {
        return uVar3 & 1;
    }
    // WARNING: Subroutine does not return
    sym.imp.__stack_chk_fail();
}
