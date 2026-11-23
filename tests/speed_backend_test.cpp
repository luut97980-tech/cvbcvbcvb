#include "game_backend.hpp"
#include "fake_il2cpp.hpp"
#include <cassert>
#include <cmath>
#include <limits>
int main(){
 using namespace fake;using namespace vt;using namespace vt::ios;
 auto&base=cls("ActorControl");auto&normal=cls("ActorDefaultControlPlayer");normal.parent=&base;
 auto&mini=cls("ActorMiniGameControl");mini.parent=&base;
 auto&ac=cls("ActorCharacter");auto&sysC=cls("ActorSystem");field(sysC,"Self",0x12,8);
 auto&controlF=field(ac,"actorControl",0x12,8);auto&uid=field(ac,"<SUID>k__BackingField",0x0a,8);
 auto&ownerF=field(base,"actorBase",0x12,8);auto&termF=field(base,"_terminate",2,1);auto&unitF=field(base,"kunit",0x12,8);
 auto&unitC=cls("KinematicControllerUnit","PlayTogether.Kinematic");
 auto&groundF=field(unitC,"_stableMoveSpd",0x0c,4);auto&airF=field(unitC,"_stableAirMoveSpeed",0x0c,4);
 auto&deadF=field(mini,"<IsDeathInGame>k__BackingField",2,1);auto&goalF=field(mini,"<IsGoalIn>k__BackingField",2,1);
 Object sys{&sysC},act{&ac},ctrl{&normal},mc{&mini},u{&unitC},mu{&unitC};
 fake::system=&sys;actor=&act;set(act,controlF,&ctrl);set(act,uid,std::int64_t(42));
 for(auto*c:{&ctrl,&mc}){set(*c,ownerF,&act);set(*c,termF,false);}
 set(ctrl,unitF,&u);set(mc,unitF,&mu);set(mc,deadF,false);set(mc,goalF,false);
 set(u,groundF,5.f);set(u,airF,4.f);set(mu,groundF,10.f);set(mu,airF,8.f);
 auto close=[](float a,float b){if(std::fabs(a-b)>=0.0001f){std::fprintf(stderr,"speed mismatch actual=%f expected=%f\n",a,b);}assert(std::fabs(a-b)<0.0001f);};
 Model m;m.available.fill(true);m.connected=m.controlsAllowed=m.license.valid=true;m.lobbySpeedTenths=12;m.gameSpeedTenths=15;
 {
 GameBackend b(resolve);auto tick=[&](double t){m.time=t;b.tick(m,t);};
 b.toggle(m,Speed);tick(1);close(get<float>(u,groundF),6.f);close(get<float>(u,airF),4.8f);
 tick(2);close(get<float>(u,groundF),6.f); // never multiply our own write
 m.lobbySpeedTenths=13;tick(3);close(get<float>(u,groundF),6.5f);
 set(u,groundF,7.f);tick(4);close(get<float>(u,groundF),9.1f); // follow new game baseline
 b.toggle(m,Speed);close(get<float>(u,groundF),7.f);close(get<float>(u,airF),4.f);
 b.toggle(m,Speed);tick(5);set(u,groundF,8.f);b.toggle(m,Speed);close(get<float>(u,groundF),8.f); // retain newer external change
 b.toggle(m,Speed);tick(6);set(act,controlF,&mc);tick(7);
 close(get<float>(u,groundF),8.f);close(get<float>(mu,groundF),15.f);assert(m.speedInGame);
 set(mc,deadF,true);tick(8);close(get<float>(mu,groundF),10.f);
 set(mc,deadF,false);tick(9);close(get<float>(mu,groundF),15.f);
 m.license.valid=false;tick(10);close(get<float>(mu,groundF),10.f);
 m.license.valid=true;tick(11);close(get<float>(mu,groundF),15.f);
 b.suspend(12);close(get<float>(mu,groundF),10.f);tick(14);close(get<float>(mu,groundF),10.f);tick(14.8);close(get<float>(mu,groundF),15.f);
 m.gameSpeedTenths=10;tick(15);close(get<float>(mu,groundF),10.f);
 m.gameSpeedTenths=15;tick(16);b.stop(m);close(get<float>(mu,groundF),10.f);assert(!m.enabled[Speed]);
 // Invalid values must not be propagated.
 set(mu,groundF,std::numeric_limits<float>::quiet_NaN());b.toggle(m,Speed);tick(17);assert(std::isnan(get<float>(mu,groundF)));b.stop(m);
 set(mu,groundF,10.f);b.toggle(m,Speed);tick(18);close(get<float>(mu,groundF),15.f);
 }
 close(get<float>(mu,groundF),10.f);assert(roots.empty());
}
