world {}

table (world){
    shape:ssBox, Q:<t(0 0. .8)>, size:[2.0 1.5 .1 .02], color:[.8 .8 .8], contact
}
table2 (table) {
    shape:ssBox, Q:<t(0.5 0 .3)>, size:[.3 .3 .6 .02], color:[.8 .8 .8], contact: -1
}


Prefix = "R_"
Include 'panda_convexGripper.g'
Prefix!

joint (table R_panda_link0){ joint:rigid Q:<t(-.5 .0 .05) d(0 0 0 1)> }
joint (R_panda_joint7 R_gripper){
  Q:<t(0.107 0 0.0) d(90 0 1 0) d(-45 0 0 1)>
}


Prefix = "L_"
Include 'panda_convexGripper.g'
Prefix!
joint (table2 L_panda_link0){ joint:rigid Q:<t(-0.05 .0 .3) d(180 0 0 1)> }
joint (L_panda_joint7 L_gripper){
  Q:<t(0.107 0 0.0) d(90 0 1 0) d(-45 0 0 1)>
}

