
hand {
  shape:mesh  mesh:'meshes/collision/hand.stl',  visual, color=[.9 .9 .9] 
  #shape:mesh  mesh:'meshes/visual/hand.ply',  visual, color=[.9 .9 .9] 
}

gripper (hand) {
  shape:marker, size:[.0], Q:<d(-90 0 0 1)>
}

finger1 (gripper) {
    #joint:transY Q:<> A:<t(+.05 0 .09) d(-90 0 0 1)> limits: [-.045 0.0]
    #shape:capsule, color:[ 0.2, 0.2, 0.2 ], size:[.04 .01], contact:-4
    
    #joint:transY Q:<> A:<t(+.05 0 .11) d(-90 0 0 1)> limits: [-.045 0.0]
    #shape:sphere, color:[ 0.2, 0.2, 0.2 ], size:[.015], contact:-4
    
    joint:transY Q:<> A:<t(+.04 0 0.0584) d(-90 0 0 1) > limits: [-.04 0.0]
    shape:mesh  color:[ 0.2 0.2 0.2 ]  mesh:'meshes/collision/finger.stl'
    #shape:mesh  color:[ 0.9 0.9 0.9 ]  mesh:'meshes/visual/finger.ply'
}

finger2 (gripper){
    #joint:transY mimic:(finger1) Q:<> A:<d(180 0 0 1) t(+.05 0 .09) d(-90 0 0 1)>
    #shape:capsule, color:[ 0.2, 0.2, 0.2 ], size:[.04 .01], contact:-4
    
    #joint:transY mimic:(finger1) Q:<> A:<d(180 0 0 1) t(+.05 0 .11) d(-90 0 0 1)>
    #shape:sphere, color:[ 0.2, 0.2, 0.2 ], size:[.015], contact:-4
    
    joint:transY mimic:(finger1) Q:<> A:<d(180 0 0 1) t(+.04 0 0.0584) d(-90 0 0 1)>
    shape:mesh  color:[ 0.2 0.2 0.2 ]  mesh:'meshes/collision/finger.stl'
    #shape:mesh  color:[ 0.9 0.9 0.9 ]  mesh:'meshes/visual/finger.ply'
}


gripperCenter (gripper){
    shape:marker, size:[.03], color:[.9 .9 .9],
    Q:<t(0 0 -.05) d(0 1 0 0)>
}

