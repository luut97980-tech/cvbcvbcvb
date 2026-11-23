#include "mobile_menu.hpp"
#include "imgui_internal.h"
#include <cassert>
#include <cmath>
#include <limits>
#include <map>
#include <string>
static std::map<ImGuiID,ImRect> rects;
static std::map<ImGuiID,std::string> names;
static std::map<std::string,ImRect> items;
void ImGuiTestEngineHook_ItemAdd(ImGuiContext*,ImGuiID id,const ImRect&r,const ImGuiLastItemData*){rects[id]=r;if(names.contains(id))items[names[id]]=r;}
void ImGuiTestEngineHook_ItemInfo(ImGuiContext*,ImGuiID id,const char*label,ImGuiItemStatusFlags){names[id]=label;if(rects.contains(id))items[label]=rects[id];}
void ImGuiTestEngineHook_Log(ImGuiContext*,const char*,...){}
const char* ImGuiTestEngine_FindItemDebugLabel(ImGuiContext*,ImGuiID){return "three-features";}
int main(){
    using namespace vt::ios;static_assert(vt::FeatureCount==6);
    Model m;Actions a;int toggles=0,last=-1,saves=0,stops=0,copies=0;
    a.diagnostic=[&]{++copies;};
    a.toggle=[&](int id){++toggles;last=id;};a.save=[&]{++saves;};a.stop=[&]{++stops;};
    auto*context=ImGui::CreateContext();context->TestEngineHookItems=true;
    auto&io=ImGui::GetIO();io.IniFilename=nullptr;io.LogFilename=nullptr;io.DisplaySize={414,808};io.DeltaTime=1.f/60;
    unsigned char*p;int w,h;io.Fonts->GetTexDataAsRGBA32(&p,&w,&h);io.Fonts->SetTexID(1);style();
    auto frame=[&]{items.clear();rects.clear();ImGui::NewFrame();draw(m,a);ImGui::Render();};
    auto click=[&](const char*label){if(!items.contains(label)){printf("Missing item label: %s\n", label);fflush(stdout);}assert(items.contains(label));auto p=items[label].GetCenter();io.AddMousePosEvent(p.x,p.y);io.AddMouseButtonEvent(0,false);frame();io.AddMouseButtonEvent(0,true);frame();io.AddMouseButtonEvent(0,false);frame();frame();};
    
    // 1. Collapsed button test & initial position
    frame();frame();assert(!m.visible&&items.contains("Menu"));
    assert(acceptsPoint(m,items["Menu"].GetCenter())&&!acceptsPoint(m,{0,0}));

    // 2. Drag floating button test
    m.floatingButtonPos={120,200};frame();frame();
    assert(acceptsPoint(m,{130,210})&&!acceptsPoint(m,{10,10}));

    // 3. Open menu without valid key -> Key activation section shown
    click("Menu");frame();frame();
    assert(items.contains("Kích hoạt Key")&&items.contains("Dán từ bộ nhớ"));

    // 4. Activate key -> Features unlocked
    m.license.valid=true;m.license.statusText="Key Hợp Lệ • Còn 30 ngày";
    frame();frame();assert(items.contains("ESP")&&items.contains("Line")&&items.contains("Spam nhảy")&&items.contains("Tự sẵn sàng")&&items.contains("Tự động búa"));
    assert(!items.contains("Tăng tốc (Speed)")&&!items.contains("Nút rời Speed"));
    for(const char*removed:{"Nhảy cao","Nhảy đôi","Sương","Ready","TCP nhanh","Xuyên"})assert(!items.contains(removed));
    
    click("ESP");assert(toggles==0);m.connected=m.controlsAllowed=true;m.available.fill(true);frame();
    click("ESP");assert(toggles==1&&last==vt::Esp);
    click("Line");assert(toggles==2&&last==vt::Line);
    click("Spam nhảy");assert(toggles==3&&last==vt::SpamJump);
    click("Tự sẵn sàng");assert(toggles==4&&last==vt::AutoReady);
    click("Tự động búa");assert(toggles==5&&last==vt::AutoHammer);
    m.suppressActions=true;click("ESP");assert(toggles==5);m.suppressActions=false;
    click("Lưu");assert(saves==1);click("Tắt hết");assert(stops==1);

    // A drag must not activate footer buttons or feature toggles, even if it
    // returns to its starting point before release. Small tap jitter is valid.
    auto gesture=[&](const char* label, float dx, float dy, bool returnToStart){
        auto start=items.at(label).GetCenter();
        io.AddMousePosEvent(start.x,start.y);frame();
        io.AddMouseButtonEvent(0,true);frame();
        io.AddMousePosEvent(start.x+dx,start.y+dy);frame();
        if(returnToStart){io.AddMousePosEvent(start.x,start.y);frame();}
        io.AddMouseButtonEvent(0,false);frame();frame();
    };
    const int beforeStop=stops, beforeToggle=toggles;
    gesture("Tắt hết",24,0,false);assert(stops==beforeStop);
    gesture("Tắt hết",0,30,true);assert(stops==beforeStop);
    gesture("Tắt hết",30,30,true);assert(stops==beforeStop);
    gesture("ESP",30,0,true);assert(toggles==beforeToggle);
    gesture("Tắt hết",3,2,false);assert(stops==beforeStop+1);
    click("Tắt hết");assert(stops==beforeStop+2);
    assert(!items.contains("Sao chép chẩn đoán"));
    
    // 5. Details & Anti-Capture toggle
    click("Chi tiết");frame();frame();
    assert(m.showDetails&&items.contains("Sao chép chẩn đoán")&&items.contains("Ẩn trên bản chia sẻ: BẬT"));
    click("Ẩn trên bản chia sẻ: BẬT");frame();frame();
    assert(items.contains("Ẩn trên bản chia sẻ: TẮT")&&!m.antiCapture);
    click("Ẩn trên bản chia sẻ: TẮT");frame();frame();
    assert(items.contains("Ẩn trên bản chia sẻ: BẬT")&&m.antiCapture);
    click("Sao chép chẩn đoán");assert(copies==1);
    click("Chi tiết");frame();frame();assert(!m.showDetails&&!items.contains("Sao chép chẩn đoán"));
    click("Ẩn");frame();assert(!m.visible);
    
    // 6. Responsive sizes
    m.visible=true;m.player="42";
    for(ImVec2 size:{ImVec2(375,724),ImVec2(808,375),ImVec2(1024,1320),ImVec2(1366,980),ImVec2(507,1000),ImVec2(896,414),ImVec2(414,896),ImVec2(1024,768),ImVec2(768,1024)}){
        io.DisplaySize=size;frame();frame();assert(items.contains("Lưu"));assert(items["Lưu"].Max.y<=size.y+1);
        assert(acceptsPoint(m,items["Lưu"].GetCenter())&&!acceptsPoint(m,{0,0}));
        assert(m.windowMax.x-m.windowMin.x>m.windowMax.y-m.windowMin.y);
        assert(items["ESP"].Max.x<=m.windowMax.x&&items["Line"].Max.x<=m.windowMax.x&&items["Spam nhảy"].Max.y<=m.windowMax.y&&items["Tự sẵn sàng"].Max.y<=m.windowMax.y);
    }
    io.DisplaySize={896,414};m.insetMin={44,0};m.insetMax={44,21};m.scale=1.25f;
    frame();frame();assert(m.windowMin.x>=44&&m.windowMax.x<=852&&m.windowMax.y<=393);
    click("Spam nhảy");assert(last==vt::SpamJump);
    assert(items["Lưu"].Max.y<=393&&!io.MouseDrawCursor);
    m.jumpIntervalMs=std::numeric_limits<double>::quiet_NaN();sanitize(m);assert(m.jumpIntervalMs==20);
    m.jumpIntervalMs=-1;sanitize(m);assert(m.jumpIntervalMs==0);
    m.jumpIntervalMs=99999;sanitize(m);assert(m.jumpIntervalMs==1000);
    // Two independent speed profiles, exact tenths, bounded inputs and floating toggle.
    io.DisplaySize={1024,1320};m.insetMin=m.insetMax={};m.scale=1;m.menuWindowPos={10,10};
    m.speedButtonPos={850,100};m.visible=m.showDetails=true;m.license.valid=true;
    m.lobbySpeedTenths=m.gameSpeedTenths=10;frame();frame();
    click("+0.1##LobbySpeed");assert(m.lobbySpeedTenths==11&&m.gameSpeedTenths==10);
    click("+0.1##GameSpeed");assert(m.gameSpeedTenths==11&&m.lobbySpeedTenths==11);
    click("-0.1##LobbySpeed");assert(m.lobbySpeedTenths==10&&m.gameSpeedTenths==11);
    for(int i=0;i<30;++i)click("+0.1##GameSpeed");assert(m.gameSpeedTenths==20);
    for(int i=0;i<30;++i)click("-0.1##GameSpeed");assert(m.gameSpeedTenths==10);
    m.visible=false;frame();frame();
    assert(acceptsPoint(m,{860,110}));
    click("Speed OFF 1.0x##SpeedFloatingToggle");assert(last==vt::Speed);
    m.license.valid=false;frame();assert(m.speedButtonMax.x==0);
    ImGui::DestroyContext(context);
}
