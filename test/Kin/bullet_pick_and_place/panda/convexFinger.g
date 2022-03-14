
finger { shape:mesh  mesh:'finger/finger_0.obj',  color=[.9 .9 .9], contact: 0}

#finger_coll (finger) {  
    #shape:mesh mesh:'meshes/collision/finger.stl' 
    #color:[ 0.8 0.2 0.2 0. ] noVisual, contact:-2 
#}

finger_coll1 (finger) {  
    shape:sphere size:[.02] Q:<t(0.0 0.008 0.045)>
    color:[ 0.9 0.9 0.9 0.1 ] noVisual, contact:-2 
}

finger_coll2 (finger) {  
    shape:sphere size:[.02] Q:<t(0.0 0.02 0.02)>
    color:[ 0.9 0.9 0.9 0.1 ] noVisual, contact:-2 
}

finger_1 (finger) { shape:mesh  mesh:'finger/finger_1.obj',  color=[.9 .9 .9], contact: 0}
finger_2 (finger) { shape:mesh  mesh:'finger/finger_2.obj',  color=[.9 .9 .9], contact: 0}
finger_3 (finger) { shape:mesh  mesh:'finger/finger_3.obj',  color=[.9 .9 .9], contact: 0}




#finger_3 (finger) { 
#    shape:ssBox  size: [0.02 0.02 0.02 0.005] 
#    Q:<t(0.0 0.008 .0456)> color=[.9 .9 .9 1.], contact: 1
#}

#graspCheck (finger) {
  #shape: sphere, size:[0.009],  
#  shape: ssBox, size:[0.018 0.018 0.018 0.005],  
#  Q:<t(0. 0.008 .0456)>, color: [.8 .5 .5 .5], contact:-4
#}
