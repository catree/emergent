// Copyright, 1995-2013, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of The Emergent Toolkit
//
//   This library is free software; you can redistribute it and/or
//   modify it under the terms of the GNU Lesser General Public
//   License as published by the Free Software Foundation; either
//   version 2.1 of the License, or (at your option) any later version.
//
//   This library is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//   Lesser General Public License for more details.

#include "VEWorldView.h"
#include <VEWorld>
#include <iViewPanelOfVEWorld>
#include <VESpaceView>
#include <VESpace>
#include <iT3Panel>
#include <T3ExaminerViewer>
#include <T3VEWorld>
#include <taSigLinkItr>
#include <taImage>
#include <NewViewHelper>
#include <T3Panel>
#include <taMath_float>

#include <VEObjectView>
#include <VEObject>

#include <taMisc>


#ifdef TA_QT3D

#else // TA_QT3D
#include <SoOffscreenRendererQt>
#include <Inventor/nodes/SoTexture2.h>
#include <Inventor/nodes/SoTexture2Transform.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/nodes/SoPointLight.h>
#include <Inventor/nodes/SoSpotLight.h>
#include <Inventor/SoInput.h>
#include <Inventor/SoDB.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/SbViewportRegion.h>
#include <Inventor/VRMLnodes/SoVRMLImageTexture.h>
#include <Inventor/elements/SoGLCacheContextElement.h>
#endif // TA_QT3D


TA_BASEFUNS_CTORS_DEFN(VEWorldView);



bool VEWorldView::isVisible() const {
  return (taMisc::use_gui );
}

void VEWorldView::Initialize() {
  display_on = true;
  drag_objs = true;
  drag_size = 0.1f;
  show_joints = true;
  data_base = &TA_VEWorld;
//   children.SetBaseType(&TA_VEObjectView);
#ifdef TA_QT3D

#else // TA_QT3D
  cam_renderer = NULL;
#endif // TA_QT3D
  nowin_rebuild_done = false;
}

void VEWorldView::InitLinks() {
  inherited::InitLinks();
//   taBase::Own(table_orient, this);
}

void VEWorldView::CutLinks() {
  inherited::CutLinks();
#ifdef TA_QT3D

#else // TA_QT3D
  if(cam_renderer) delete cam_renderer;
  cam_renderer = NULL;
#endif // TA_QT3D
}

void VEWorldView::Copy_(const VEWorldView& cp) {
  display_on = cp.display_on;
  drag_objs = cp.drag_objs;
  drag_size = cp.drag_size;
}

void VEWorldView::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
}

void VEWorldView::UpdateName() {
  VEWorld* wl = World();
  if (wl) {
    if (!name.contains(wl->name))
      SetName(wl->name + "_View");
  }
  else {
    if (name.empty())
      SetName("no_world");
  }
}

void VEWorldView::SigRecvUpdateAfterEdit_impl() {
  UpdateName();
  inherited::SigRecvUpdateAfterEdit_impl();
}

const String VEWorldView::caption() const {
  VEWorld* wl = World();
  String rval;
  if (wl) {
    rval = wl->GetDisplayName();
  } else rval = "(no world)";
  return rval;
}

void VEWorldView::SetWorld(VEWorld* wl) {
  if (World() == wl) return;
  if (wl) {
    SetData(wl);
    if (m_wvp) m_wvp->UpdatePanel(); // to update name
  } else {
    Unbind(); // also does kids
  }
}

void VEWorldView::OnWindowBind_impl(iT3Panel* vw) {
  inherited::OnWindowBind_impl(vw);
  if (!m_wvp) {
    m_wvp = new iViewPanelOfVEWorld(this);
    vw->RegisterPanel(m_wvp);
  }
}

void VEWorldView::InitDisplay(bool init_panel) {
  if (init_panel) {
    InitPanel();
    UpdatePanel();
  }
  // descend into sub items
//   FOREACH_ELEM_IN_GROUP(LayerView, lv, children) {
//     InitDisplay_Layer(lv, false);
//   }
}

void VEWorldView::SigRecvUpdateView_impl() {
  if(!display_on) return;
  UpdateDisplay(true);
}

void VEWorldView::UpdateDisplay(bool update_panel) {
  if (update_panel) UpdatePanel();
  Render_impl();
}

void VEWorldView::InitPanel() {
  if (m_wvp)
    m_wvp->InitPanel();
}

void VEWorldView::UpdatePanel() {
  if (m_wvp)
    m_wvp->UpdatePanel();
}

void VEWorldView::BuildAll() {
  Reset();
  VEWorld* wl = World();
  if(!wl) return;

  // do background spaces first..
  FOREACH_ELEM_IN_GROUP(VESpace, spc, wl->spaces) {
    VESpaceView* ov = new VESpaceView();
    ov->SetSpace(spc);
    children.Add(ov);
    ov->BuildAll();
  }

  FOREACH_ELEM_IN_GROUP(VEObject, obj, wl->objects) {
    VEObjectView* ov = new VEObjectView();
    ov->SetObject(obj);
    children.Add(ov);
    ov->BuildAll();
  }

  BuildAnnotations();
}

void VEWorldView::Render_pre() {
  InitPanel();

  T3ExaminerViewer* vw = GetViewer();
  if(vw) {
    vw->syncViewerMode();
  }

#ifdef TA_QT3D
  setNode(new T3VEWorld(NULL, this));
#else // TA_QT3D
  // this delay is deadly for viewing!
  SoVRMLImageTexture::setDelayFetchURL(false);
  setNode(new T3VEWorld(this));
#endif // TA_QT3D

  CreateLights();
  CreateTextures();

  inherited::Render_pre();
  // taMisc::Info("WorldView", name, "Render_pre complete");
}

void VEWorldView::CreateLights() {
  T3VEWorld* obv = (T3VEWorld*)this->node_so(); // cache
  VEWorld* wl = World();

#ifdef TA_QT3D
#else // TA_QT3D
  SoGroup* lgt_group = obv->getLightGroup();
  int n_lgt = 0;
  if(wl->light_0) {
    VELight* vlgt = wl->light_0.ptr();
    SoLight* lt = vlgt->CreateLight();
    lgt_group->addChild(lt);
    n_lgt++;
  }
  if((n_lgt == 1) && (bool)wl->light_1) {
    VELight* vlgt = wl->light_1.ptr();
    SoLight* lt = vlgt->CreateLight();
    lgt_group->addChild(lt);
    n_lgt++;
  }
#endif // TA_QT3D
}

void VEWorldView::CreateTextures() {
  T3VEWorld* obv = (T3VEWorld*)this->node_so(); // cache
  VEWorld* wl = World();

#ifdef TA_QT3D
#else // TA_QT3D
  SoSwitch* texsw = obv->getTextureSwitch();
  SoSwitch* texxfsw = obv->getTextureXformSwitch();
  for(int i=0;i<wl->textures.size;i++) {
    VETexture* vtex = wl->textures.FastEl(i);
    SoTexture2* tex = new SoTexture2;
    vtex->SetTexture(tex);
    texsw->addChild(tex);

    SoTexture2Transform* texxf = new SoTexture2Transform;
    vtex->SetTransform(texxf);
    texxfsw->addChild(texxf);
  }
#endif // TA_QT3D
}

void VEWorldView::Render_impl() {
  inherited::Render_impl();

  // these tests are so the subroutines don't need them
  T3VEWorld* obv = (T3VEWorld*)this->node_so(); // cache
  if(!obv) return;
  VEWorld* wl = World();
  if(!wl) return;

  // don't set the caption!!  just gets in the way!
//   SoFont* font = obv->captionFont(true);
//   float font_size = 0.4f;
//   font->size.setValue(font_size); // is in same units as geometry units of network
//   obv->setCaption(caption().chars());

  obv->setShadows(wl->shadows.on, wl->shadows.precision, wl->shadows.quality,
                  wl->shadows.smoothing, wl->shadows.near_radius, wl->shadows.far_radius,
                  wl->shadows.threshold, wl->shadows.epsilon);

  SetupCameras();
  SetupLights();

  UpdatePanel();
  // taMisc::Info("WorldView", name, "Render_impl complete");
}

void VEWorldView::SetupCameras() {
  T3VEWorld* obv = (T3VEWorld*)this->node_so(); // cache
  VEWorld* wl = World();

#ifdef TA_QT3D
#else // TA_QT3D
  SoSwitch* cam_switch = obv->getCameraSwitch();
  int n_cam = 0;
  VECamera* cam_light = NULL;
  if(wl->camera_0) {
    VECamera* vecam = wl->camera_0.ptr();
    if(cam_switch->getNumChildren() == 0) {
      SoPerspectiveCamera* cam = new SoPerspectiveCamera;
      vecam->ConfigCamera(cam);
      cam_switch->addChild(cam);
    }
    if(vecam->light.on)
      cam_light = vecam;
    n_cam++;
  }
  if((n_cam == 1) && (bool)wl->camera_1) {
    VECamera* vecam = wl->camera_1.ptr();
    if(cam_switch->getNumChildren() == 1) {
      SoPerspectiveCamera* cam = new SoPerspectiveCamera;
      vecam->ConfigCamera(cam);
      cam_switch->addChild(cam);
    }
    if(!cam_light && vecam->light.on)
      cam_light = vecam;
    n_cam++;
  }
  if(n_cam == 0) {
    cam_switch->removeAllChildren();
  }

  if(cam_light) {
    obv->setCamLightOn(true);
    obv->setCamLightDir(cam_light->dir_norm.x, cam_light->dir_norm.y,
                        cam_light->dir_norm.z);

    SoDirectionalLight* caml = obv->getCamLight();
    caml->intensity = cam_light->light.intensity;
    caml->color.setValue(cam_light->light.color.r, cam_light->light.color.g, cam_light->light.color.b);
  }
  else {
    obv->setCamLightOn(false);
  }
#endif // TA_QT3D

}

void VEWorldView::SetupLights() {
  T3VEWorld* obv = (T3VEWorld*)this->node_so(); // cache
  VEWorld* wl = World();

#ifdef TA_QT3D

#else // TA_QT3D
  SoGroup* lgt_group = obv->getLightGroup();
  int n_lgt = 0;
  if(lgt_group->getNumChildren() > 0) {
    if(wl->light_0) {
      VELight* vlgt = wl->light_0.ptr();
      SoLight* lt = (SoLight*)lgt_group->getChild(0);
      vlgt->ConfigLight(lt);
      n_lgt++;
    }
    if((n_lgt == 1) && (bool)wl->light_1 && (lgt_group->getNumChildren() > 1)) {
      VELight* vlgt = wl->light_1.ptr();
      SoLight* lt = (SoLight*)lgt_group->getChild(1);
      vlgt->ConfigLight(lt);
      n_lgt++;
    }
  }

  if(wl->sun_light.on) {
    obv->setSunLightOn(true);
    obv->setSunLightDir(0.0f, -1.0f, 0.0f);
    SoDirectionalLight* slt = obv->getSunLight();
    slt->intensity = wl->sun_light.intensity;
    slt->color.setValue(wl->sun_light.color.r, wl->sun_light.color.g, wl->sun_light.color.b);
  }
  else {
    obv->setSunLightOn(false);
  }
#endif // TA_QT3D
}

// void VEWorldView::setDisplay(bool value) {
//   if (display_on == value) return;
//   display_on = value;
//   UpdateDisplay(false);              //
// }

QImage VEWorld::GetCameraImage(int cam_no) {
  QImage img;

  if(cam_no == 0) {
    if(TestError(!camera_0, "GetCameraImage", "camera_0 not set -- cannot be rendered!"))
      return img;
  }
  else if(cam_no == 1) {
    if(TestError(!camera_1, "GetCameraImage", "camera_1 not set -- cannot be rendered!"))
      return img;
  }
  else {
    TestError(true, "GetCameraImage", "only 2 cameras (0 or 1) supported!");
    return img;
  }

  taSigLink* dl = sig_link();
  if(TestError(!dl, "GetCameraImage", "data link not found -- could not find views (should not happen -- please report as a bug!"))
    return img;

  taSigLinkItr itr;
  VEWorldView* el;
  FOR_DLC_EL_OF_TYPE(VEWorldView, el, dl, itr) {
    return el->GetCameraImage(cam_no);
  }

  TestError(true, "GetCameraImage", "No View of this world found -- must create View in order to get camera images");

  return img;
}

QImage VEWorldView::GetCameraImage(int cam_no) {
  QImage img;
  VEWorld* wl = World();
  if(!wl) return img;

  T3VEWorld* obv = (T3VEWorld*)this->node_so(); // cache
  if(taMisc::gui_no_win) {    // offscreen rendering mode -- need to build a new worldview
    if(!obv || !nowin_rebuild_done) {
      taMisc::Info("GetCameraImage", String(cam_no), "offscreen building");
      //      BuildAll();
      Render_pre();
      Render_impl();
      Render_post();
      obv = (T3VEWorld*)this->node_so(); // cache
      nowin_rebuild_done = true;
    }
  }
  if(!obv) {
    TestError(!taMisc::gui_active, "GetCameraImage", "no node_so for VEworld view -- need to run with no_win, not nogui!");
    return img;
  }

#ifdef TA_QT3D

#else // TA_QT3D

  SoSwitch* cam_switch = obv->getCameraSwitch();
  if(TestWarning(cam_switch->getNumChildren() <= cam_no, "GetCameraImage",
		 "cam_switch not avail")) {
    return img; 
  }

  VECamera* vecam = NULL;
  if(cam_no == 0) {
    if(TestError(!wl->camera_0, "GetCameraImage", "camera_0 not set -- cannot be rendered!"))
      return img;
    vecam = wl->camera_0.ptr();
  }
  else if(cam_no == 1) {
    if(TestError(!wl->camera_1, "GetCameraImage", "camera_1 not set -- cannot be rendered!"))
      return img;
    vecam = wl->camera_1.ptr();
  }
  else {
    TestError(true, "GetCameraImage", "only 2 cameras (0 or 1) supported!");
    return img;
  }

  taVector2i cur_img_size = vecam->img_size;

  SbViewportRegion vpreg;
  vpreg.setWindowSize(cur_img_size.x, cur_img_size.y);

  static taVector2i last_img_size;

  if(!cam_renderer) {
    taMisc::Info("GetCameraImage", String(cam_no), "cam_renderer building");
    QGLWidget* qglwidg = NULL;
    T3ExaminerViewer* exvw = GetViewer();
    if(exvw) {
      qglwidg = exvw->quarter;
    }
    cam_renderer = new SoOffscreenRendererQt(vpreg, qglwidg);
    SoGLRenderAction* action = cam_renderer->getGLRenderAction();
    action->setSmoothing(true);
    action->setTransparencyType(SoGLRenderAction::BLEND);

    last_img_size = cur_img_size;
  }

  if(cur_img_size != last_img_size) {
    taMisc::Info("GetCameraImage", String(cam_no), "new image size");
    cam_renderer->setViewportRegion(vpreg);
    last_img_size = cur_img_size;
  }

  cam_renderer->setBackgroundColor(SbColor(wl->bg_color.r, wl->bg_color.g, wl->bg_color.b));

  cam_switch->whichChild = cam_no;

  SoPerspectiveCamera* cam = (SoPerspectiveCamera*)cam_switch->getChild(cam_no);
  vecam->ConfigCamera(cam);

  // to auto compute -- probably more expensive than its worth
//   SoGetBoundingBoxAction action(vpregion);
//   action.apply(sceneroot);
//   SbBox3f box = action.getBoundingBox();

  bool ok = cam_renderer->render(obv);

  cam_switch->whichChild = -1;  // switch off for regular viewing!

  if(ok) {
    img = cam_renderer->getImage();
  }
#endif
  return img;
}


bool VEWorld::GetCameraTaImage(taImage& ta_img, int cam_no) {
  QImage img = GetCameraImage(cam_no);
  bool rval = (bool)img.isNull();
  ta_img.SetImage(img);
  return rval;
}


VEWorldView* VEWorld::NewView(T3Panel* fr) {
  return VEWorldView::New(this, fr);
}

// Add a new VEWorldView object to the frame for the given VEWorld.
VEWorldView* VEWorldView::New(VEWorld* wl, T3Panel*& fr) {
  NewViewHelper new_net_view(fr, wl, "world");
  if (!new_net_view.isValid()) return NULL;

  VEWorldView* vw = new VEWorldView;
  fr->AddView(vw);
  vw->SetWorld(wl);
  // make sure we get it all setup!
  vw->BuildAll();

  new_net_view.showFrame();
  return vw;
}


////////////////////////////////////////
//      VEWorld extras

VEWorldView* VEWorld::FindView() {
  taSigLink* dl = sig_link();
  if(dl) {
    taSigLinkItr itr;
    VEWorldView* el;
    FOR_DLC_EL_OF_TYPE(VEWorldView, el, dl, itr) {
//       if(!el->node_so()) continue;
      return el;
    }
  }
  return NULL;
}

void VEWorld::UpdateView() {
  VEWorldView* vew = FindView();
  if(!vew) return;
  vew->UpdateDisplay(false);    // no update panel
}


