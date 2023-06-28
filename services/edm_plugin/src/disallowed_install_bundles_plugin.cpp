/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "disallowed_install_bundles_plugin.h"

#include "array_string_serializer.h"
#include "edm_ipc_interface_code.h"
#include "iplugin_manager.h"

namespace OHOS {
namespace EDM {
const bool REGISTER_RESULT = IPluginManager::GetInstance()->AddPlugin(DisallowedInstallBundlesPlugin::GetPlugin());

void DisallowedInstallBundlesPlugin::InitPlugin(
    std::shared_ptr<IPluginTemplate<DisallowedInstallBundlesPlugin, std::vector<std::string>>> ptr)
{
    EDMLOGD("DisallowedInstallBundlesPlugin InitPlugin...");
    ptr->InitAttribute(EdmInterfaceCode::DISALLOWED_INSTALL_BUNDLES, "disallowed_install_bundles",
        "ohos.permission.ENTERPRISE_SET_BUNDLE_INSTALL_POLICY", IPlugin::PermissionType::SUPER_DEVICE_ADMIN, true);
    ptr->SetSerializer(ArrayStringSerializer::GetInstance());
    ptr->SetOnHandlePolicyListener(&DisallowedInstallBundlesPlugin::OnSetPolicy, FuncOperateType::SET);
    ptr->SetOnHandlePolicyListener(&DisallowedInstallBundlesPlugin::OnRemovePolicy, FuncOperateType::REMOVE);
    ptr->SetOnAdminRemoveDoneListener(&DisallowedInstallBundlesPlugin::OnAdminRemoveDone);
    SetAppInstallControlRuleType(AppExecFwk::AppInstallControlRuleType::DISALLOWED_INSTALL);
}

ErrCode DisallowedInstallBundlesPlugin::OnGetPolicy(std::string &policyData, MessageParcel &data, MessageParcel &reply,
    int32_t userId)
{
    EDMLOGI("DisallowedInstallBundlesPlugin OnGetPolicy policyData : %{public}s, userId : %{public}d",
        policyData.c_str(), userId);
    return GetBundlePolicy(policyData, data, reply, userId);
}
} // namespace EDM
} // namespace OHOS
