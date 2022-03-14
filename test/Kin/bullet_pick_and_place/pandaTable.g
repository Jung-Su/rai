
world {}

table {
    shape:ssBox, X:<t(0 0. .8)>, size:[2.0 1.5 .1 .02], color:[.8 .8 .8], contact
}

Include: '../../../../rai-robotModels/panda_convexGripper/panda_convexGripper.g'


stick {
    shape:ssBox, X:<t(0 0 1.)>, size:[0.5 0.05 0.05 .01], color:[.4 .8 .4],
    mass=.1, friction=10.
}
