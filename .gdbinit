define regs
disp/4xw sp
disp/a $esp
disp/a $esi
disp/a $edi
disp/a $edx
disp/a $ecx
disp/a $ebx
disp/a $eax
disp/i $eip
end

define stk
x/a $esp
end

regs

break mu_zzz

#set args -d debug -f arm.mu4 -f ARM/module.mu4




