  #
  # Author: Anna Neander (2021)
  #

.align 2
.text

.global enable_interrupt

#
# executes the ei instruction
#
enable_interrupt:
  ei   # enable global interrupts
  jr	$ra
  nop  # delay slot
