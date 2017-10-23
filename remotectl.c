/*http://blog.csdn.net/bamlook/article/details/11645689*/














static int __devinit remotectl_probe(struct platform_device *pdev)
{
  struct rkxx_remotectl_platform_data *pdata = pdev->dev.platform_data;
  struct rkxx_remotectl_drvdata = *ddata;
  struct input_dev *input;
  int i, j;
  int irq;
  int err = 0;

  printk("%s\n", __func__);
  if(!pdata)
    return -EINVAL;

#ifdef CONFIG_RK30_KEYBOARD_LED_CTL
  rk29_keyboard_led_init();
#endif
  ddata = kzalloc(sizeof(struct rkxx_remotectl_drvdata), GFP_KERNEL);
  memset(ddata, 0, sizeof(struct rkxx_remotectl_drvdata));

  ddata->state = RMC_PRELOAD;
  input = input_allocata_device();
  if(!ddata || !input) {
    err = -ENOMEM;
    goto fail0;
  }

  platform_set_drvdata(pdev, ddata);

  input->name = pdev->name;
  input->phys = "gpio-keys/input0";
  input->dev.parent = &pdev->dev;
  input->id.bustype = BUS_HOST;
  input->id.vendor = 0x0001;
  input->id.product = 0x0001;
  input->id.version = 0x0100;

  /*enable auto repeat feature of linux input subsystem*/
  if(pdata->rep)
    __set_bit(EV_REP, input->evbit);

  ddata->nbuttons = pdata->nbuttons;
  ddata->input = input;
  wake_lock_init(&ddata->remotectl_wake_lock, WAKE_LOCK_SUSPEND, "rk29_remote");
  if(pdata->set_iomux)
    pdata->set_iomux();

  err = gpio_request(pdata->gpio, "remotectl");
  if(err < 0) {
    printk("gpio-keys:failed to request GPIO %d, err %d\n", pdata->gpio, err);
  }

  err = gpio_direction_input(pdata->gpio);
  if(err < 0) {
    pr_err("gpio-keys: failed to config input direction for GPIO %d, err %d\n", pdata->gpio, err);
    gpio_free(pdata->gpio);
  }

  irq = gpio_to_irq(pdata->gpio);
  if(irq < 0) {
    err = irq;
    pr_err("gpio-keys:unable to get irq number for GPIO %d, err %d\n", pdata->gpio, err);
    gpio_free(pdata->gpio);
    goto fail1;
  }

  err = request_irq(irq, remotectl_isr, IRQF_TRIGGER_FALLING, "remotectl", ddata);
  if(err) {
    pr_err("gpio-remotectl:unable to claim irq %d, err %d\n", irq, err);
    gpio_free(pdata->gpio);
    goto fail1;
  }

  setup_timer(&ddata->timer, remotectl_timer, (unsigned long)ddata);
  tasklet_init(&ddata->remote_tasklet, remotectl_do_something, (unsigned long)ddata);

  for(j = 0; j < sizeof(remotectl_button) / sizeof(struct rkxx_remotectl_button); j++) {
    printk("remotectl probe j = 0x%x\n", j);
    for(i = 0; i < remotectl_button[j].nbuttons; i++) {
      unsigned int type = EV_KEY;
      input_set_capability(input, type, remotectl_button[j].key_table[i].keyCode);
    }
  }

  err = input_register_device(input);
  if(err) {
    pr_err("gpio-keys:unable to register input device, err %d\n", err);
    goto fail2;
  }

  input_set_capability(input, EV_KEY, KEY_WAKEUP);
  device_init_wakeup(&pdev->dev, 1);
  return 0;

fail2:
  pr_err("gpio-remotectl input_allocata_device failed\n");
  input_free_device(input);//如果都已经分配失败了，即input = NULL,还可以free吗？
  kfree(ddata);

fail1:
  pr_err("gpio-remotectl input_allocata_device failed\n");
  free_irq(gpio_to_irq(pdata->gpio), ddata);
  del_timer_sync(&ddata->timer);
  tasklet_kill(&ddata->remote_tasklet);
  gpio_free(pdata->gpio);

fail0:
  pr_err("gpio-remotectl input_allocata_device failed\n");
//  input_free_device(input);//如果都已经分配失败了，即input = NULL,还可以free吗？
//  kfree(ddata);

  return err;
}


static int __devexit remotectl_remove(struct platform_device *pdev)
{
  struct rkxx_remotectl_platform_data *pdata = pdev->dev.platform_data;
  struct rkxx_remotectl_drvdata *ddata = platform_get_drvdata(pdev);
  struct input_dev *input = ddata->input;

  int irq;

  device_init_wakeup(&pdev->dev, 0);
  irq = gpio_to_irq(pdata->gpio);
  free_irq(irq, ddata);
  tasklet_kill(&ddata->remote_tasklet);
  gpio_free(pdata->gpio);

  input_unregister_device(input);

  return 0;
}

#ifdef CONFIG_PM

static int remotectl_suspend(struct device *dev)
{
  struct platform_device *pdev = to_platform_device(dev);
  struct rkxx_remotectl_platform_data *pdata = pdev->dev.platform_data;
  struct rkxx_remotectl_drvdata *ddata = platform_get_drvdata(pdev);

  ddata->remotectl_suspend_data.cnt = 0;
  if(device_may_wakeup(&pdev->dev)) {
    if(pdata->wakeup) {
      int irq = gpio_to_irq(pdata->gpio);
      enable_irq_wake(irq);
    }
  }
  return 0;
}


static const struct dev_pm_ops remotectl_pm_ops = {
  .suspend = remotectl_suspend,
  .resume = remotectl_resume,
};

#endif


static platform_driver remotectl_device_driver = {
  .probe = remotectl_probe,
  .remove = __devexit_p(remotectl_remove),
  .driver = {
    .name = "rkxx_remotectl",
    .owner = THIS_MODULE,
#ifdef CONFIG_PM
    .pm = &remotectl_pm_ops,
#endif
  },
};

static int remotectl_init(void)
{
  return platform_driver_register();
}
module_init(remotectl_init);

static void remotectl_exit(void)
{

}
module_exit(remotectl_exit);
MODULE_LIENSE("GPL");
