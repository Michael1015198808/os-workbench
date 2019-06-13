void spin_test(){
    _intr_write(0);
    kmt->spin_lock(&test_spin);
    kmt->spin_lock(&test_spin);
    kmt->spin_unlock(&test_spin);
    kmt->spin_unlock(&test_spin);
    for(volatile int i=0;i<1000;++i);
    kmt->spin_lock(&test_spin);
    printf("cpu %d gets the final lock\n",_cpu());
    while(1);
}
