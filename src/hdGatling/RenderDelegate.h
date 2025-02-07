//
// Copyright (C) 2019-2022 Pablo Delgado Krämer
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <https://www.gnu.org/licenses/>.
//

#pragma once

#include <pxr/imaging/hd/renderDelegate.h>

#include "MaterialNetworkTranslator.h"

struct GiScene;

PXR_NAMESPACE_OPEN_SCOPE

class HdGatlingRenderDelegate final : public HdRenderDelegate
{
public:
  HdGatlingRenderDelegate(const HdRenderSettingsMap& settingsMap,
                          const MaterialNetworkTranslator& translator);

  ~HdGatlingRenderDelegate() override;

public:
  HdRenderSettingDescriptorList GetRenderSettingDescriptors() const override;

  void SetRenderSetting(TfToken const& key, VtValue const& value) override;

public:
  HdRenderPassSharedPtr CreateRenderPass(HdRenderIndex* index,
                                         const HdRprimCollection& collection) override;

  HdResourceRegistrySharedPtr GetResourceRegistry() const override;

  void CommitResources(HdChangeTracker* tracker) override;

  HdInstancer* CreateInstancer(HdSceneDelegate* delegate,
                               const SdfPath& id) override;

  void DestroyInstancer(HdInstancer* instancer) override;

  HdAovDescriptor GetDefaultAovDescriptor(const TfToken& name) const override;

  HdRenderParam* GetRenderParam() const override;

public:
  // Rprim
  const TfTokenVector& GetSupportedRprimTypes() const override;

  HdRprim* CreateRprim(const TfToken& typeId, const SdfPath& rprimId) override;

  void DestroyRprim(HdRprim* rPrim) override;

  // Sprim
  const TfTokenVector& GetSupportedSprimTypes() const override;

  HdSprim* CreateSprim(const TfToken& typeId, const SdfPath& sprimId) override;

  HdSprim* CreateFallbackSprim(const TfToken& typeId) override;

  void DestroySprim(HdSprim* sprim) override;

  // Bprim
  const TfTokenVector& GetSupportedBprimTypes() const override;

  HdBprim* CreateBprim(const TfToken& typeId, const SdfPath& bprimId) override;

  HdBprim* CreateFallbackBprim(const TfToken& typeId) override;

  void DestroyBprim(HdBprim* bprim) override;

public:
  TfToken GetMaterialBindingPurpose() const override;

  // In a USD file, there can be multiple networks associated with a material:
  //   token outputs:mdl:surface.connect = </Root/Glass.outputs:out>
  //   token outputs:surface.connect = </Root/GlassPreviewSurface.outputs:surface>
  // This function returns the order of preference used when selecting one for rendering.
  TfTokenVector GetMaterialRenderContexts() const override;

  TfTokenVector GetShaderSourceTypes() const override;

private:
  const MaterialNetworkTranslator& m_translator;
  HdResourceRegistrySharedPtr m_resourceRegistry;
  HdRenderSettingDescriptorList m_settingDescriptors;
  HdRenderSettingDescriptorList m_debugSettingDescriptors;
  std::unique_ptr<HdRenderParam> m_renderParam;
  GiScene* m_giScene = nullptr;
};

PXR_NAMESPACE_CLOSE_SCOPE
