
// WARNING: Variable defined which should be unmapped: var_10h
// WARNING: Variable defined which should be unmapped: var_20h

void sym.tracer_disable_upload(void)
{
    int64_t iVar1;
    int64_t iVar2;
    int64_t *piVar3;
    int64_t iVar4;
    int64_t var_20h;
    int64_t var_10h;
    
    iVar2 = *(int64_t *)0xc57b0;
    iVar1 = tpidr_el0;
    iVar4 = *(int64_t *)(iVar1 + 0x28);
    fcn.00073d24(*(int64_t *)0xc57b0);
    piVar3 = *(int64_t **)(iVar2 + 0x90);
    if (piVar3 != (int64_t *)0x0) {
        (**(code **)(*piVar3 + 0x38))(piVar3, 1);
    }
    fcn.00073dbc(0xca868);
    if (*(int64_t *)(iVar1 + 0x28) == iVar4) {
        return;
    }
    // WARNING: Subroutine does not return
    sym.imp.__stack_chk_fail();
}
