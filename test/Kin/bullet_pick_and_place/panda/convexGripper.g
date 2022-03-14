
gripper {
  shape:mesh  mesh:'meshes/visual/hand.ply'  visual, color=[.9 .9 .9]
  #shape:mesh  color:[ 0.9 0.9 0.9 0.1]  mesh:'meshes/collision/hand.stl' 
}


Prefix: "L_"
Include: 'convexFinger.g'
Prefix!

Prefix: "R_"
Include: 'convexFinger.g'
Prefix!

Edit L_finger (gripper) {
  joint:transY Q:<> A:<t(0 +.05 0.0584)> limits: [-.05 0.0] color=[.9 .5 .5], finger
}

Edit R_finger (gripper) {
  joint:transY Q:<> A:<d(180 0 0 1) t(0 +.05 0.0584)> mimic:(L_finger) 
}

Prefix: "L2_"
Include: 'convexFinger.g'
Prefix!

Prefix: "R2_"
Include: 'convexFinger.g'
Prefix!

Edit L2_finger (gripper) {
  joint:transY Q:<> A:<t(0.05 +.05 0.0584)> limits: [-.05 0.0] color=[.9 .5 .5], finger
}

Edit R2_finger (gripper) {
  joint:transY Q:<> A:<d(180 0 0 1) t(-0.05 +.05 0.0584)> mimic:(L2_finger) 
}

Prefix: "L3_"
Include: 'convexFinger.g'
Prefix!

Prefix: "R3_"
Include: 'convexFinger.g'
Prefix!

Edit L3_finger (gripper) {
  joint:transY Q:<> A:<t(-0.05 +.05 0.0584)> limits: [-.05 0.0] color=[.9 .5 .5], finger
}

Edit R3_finger (gripper) {
  joint:transY Q:<> A:<d(180 0 0 1) t( 0.05 +.05 0.0584)> mimic:(L3_finger) 
}



#gripper_vis (gripper) {
# shape:mesh  mesh:'meshes/visual/hand.ply'  visual, color=[.9 .9 .9]
#}

gripper_coll (gripper) {
  shape: capsule size:[0.18 .03] Q:<t(0.0 0.00 0.04) d(90 1 0 0)> 
  color:[ 0.9 0.9 0.9 0.1 ] noVisual, contact:-2
}

gripperCenter (gripper){
    shape:marker, size:[.05], color:[.9 .9 .9],
    Q:<t(0 0 .104) d(-90 0 0 1) d(180 1 0 0)>
}
