
Prefix: "l_"
Include: '../../../../rai-robotModels/scenarios/panda_fixRobotiq.g'
Prefix: "r_"
Include: '../../../../rai-robotModels/scenarios/panda_fixRobotiq.g'
Prefix!
Edit l_panda_base { X:<t(-.4 -.3 .9) d(90 0 0 1)> , motors}
Edit r_panda_base { X:<t( .4 -.3 .9) d(90 0 0 1)>}
table2 {X:[0 0 0.8], shape: ssBox, size:[2.3 1.24 .1 .02], color:[.3 .3 .3],
contact, logical:{ }
}

box { shape:ssBox, size:[.3 .3 .3 .05], X:[1.3 0 2.5], mass=3 }
