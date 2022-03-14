#include <Kin/frame.h>
#include <Kin/kin_bullet.h>
#include <Kin/viewer.h>
#include <KOMO/komo.h>
#include <Core/thread.h>

//===========================================================================

void simGfile(){
    //-- load configuration
    rai::Configuration RealWorld("pandaTable.g");
    RealWorld.optimizeTree();

    BulletInterface bull_rai(RealWorld);
    FrameL bots = RealWorld.getParts();
    for(auto& f:bots) if((*f->ats)["motors"]){
      bull_rai.motorizeMultiBody(f);
    }

    RealWorld.watch(true);
    RealWorld.gl()->resetPressedKey();
    double tau = .01;
    double glTau = .025;
    for(uint t=0;t<200;t++){
      bull_rai.step(tau);
      bull_rai.pullDynamicStates(RealWorld);
      RealWorld.watch(false);
    }

    rai::Configuration C("pandaTable.g");
    C.optimizeTree();
    C["stick"]->setPose(rai::Transformation(RealWorld["stick"]->getPose()));
    C.selectJointsByName({"panda_joint1", "panda_joint2", "panda_joint3", "panda_joint4", "panda_joint5", "panda_joint6", "panda_joint7"});

    cout << C.getJointNames() << endl;
    arr q0 = C.getJointState();
    KOMO komo;
    komo.setModel(C);
    komo.setTiming(1., 10, 1., 2);
    komo.add_qControlObjective({}, 2);
    komo.add_qControlObjective({}, 1);
    komo.addObjective({1.}, FS_qItself, {}, OT_eq, {1e2}, NoArr, 1);
    komo.addObjective({1.}, FS_positionRel, {"gripperCenter", "stick"}, OT_eq, {1e2}, {0, 0, .0});
    komo.addObjective({1.}, FS_scalarProductXX, {"stick", "gripperCenter"}, OT_eq, {1e2});
    komo.addObjective({1.}, FS_scalarProductXZ, {"stick", "gripperCenter"}, OT_eq, {1e2});
    komo.addObjective({1.}, FS_scalarProductZZ, {"world", "gripperCenter"}, OT_eq, {1e2}, {1.});
    komo.optimize();
//    komo.view(true, "optimized configuration"); //display it
//    komo.view_play();

    arr q = komo.getConfiguration_qOrg(komo.T-1);
    C.setJointState(q);

    KOMO komo2;
    komo2.setModel(C);
    komo2.setTiming(1., 10, 1., 2);
    komo2.add_qControlObjective({}, 2);
    komo2.add_qControlObjective({}, 1);
    komo2.addObjective({1.}, FS_qItself, {C.getJointNames()}, OT_eq, {1e2}, q0);
    komo2.optimize();

    Metronome tic(tau);
    for(uint t=0;t<1000;t++){
      if(t<100){
        arr q = komo.getConfiguration_qOrg(int(t/10));
        C.setJointState(q);
        bull_rai.setMotorQ(C);
      }else if(t==100){
        bull_rai.setFinger("L_finger", 0., .01, .2);
      }else if(t>200 && t<300){
        arr q = komo2.getConfiguration_qOrg(int((t-200)/10));
        C.setJointState(q);
        bull_rai.setMotorQ(C);
      }else if(t>600 && t<700){
        arr q = komo.getConfiguration_qOrg(int((t-600)/10));
        C.setJointState(q);
        bull_rai.setMotorQ(C);
      }else if(t==700){
        bull_rai.setFinger("L_finger", 1., .01, .2);
      }else if(t>800 && t<900){
        arr q = komo2.getConfiguration_qOrg(int((t-800)/10));
        C.setJointState(q);
        bull_rai.setMotorQ(C);
      }

      bull_rai.step(tau);
      bull_rai.pullDynamicStates(RealWorld);

      if(tau>glTau || !(t%int(glTau/tau))){
        int key = RealWorld.watch(false, STRING("time t="<<tau*t));
        if(key==13 || key==27 || key=='q') break;
      }

      tic.waitForTic();
    }

}

//===========================================================================

int main(int argc,char** argv){
  rai::initCmdLine(argc,argv);

//  dropRandomScene();
  simGfile();

  return 0;
}


