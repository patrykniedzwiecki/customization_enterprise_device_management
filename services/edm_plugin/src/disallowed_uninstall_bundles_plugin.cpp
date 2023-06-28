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

#include "disallowed_uninstall_bundles_plugin.h"

#include "array_string_serializer.h"
#include "edm_ipc_interface_code.h"
#include "iplugin_manager.h"

namespace OHOS {
namespace EDM {
const bool REGISTER_RESULT = IPluginManager::GetInstance()->AddPlugin(DisallowedUninstallBundlesPlugin::GetPlugin());

void DisallowedUninstallBundlesPlugin::InitPlugin(
    std::shared_ptr<IPluginTemplate<DisallowedUninstallBundlesPlugin, std::vector<std::string>>> ptr)
{
    EDMLOGD("DisallowedUninstallBundlesPlugin InitPlugin...");
    ptr->InitAttribute(EdmInterfaceCode::DISALLOWED_UNINSTALL_BUNDLES, "disallowed_uninstall_bundles",
        "ohos.permission.ENTERPRISE_SET_BUNDLE_INSTALL_POLICY", IPlugin::PermissionType::SUPER_DEVICE_ADMIN, true);
    ptr->SetSerializer(ArrayStringSerializer::GetInstance());
    ptr->SetOnHandlePolicyListener(&DisallowedUninstallBundlesPlugin::OnSetPolicy, FuncOperateType::SET);
    ptr->SetOnHandlePolicyListener(&DisallowedUninstallBundlesPlugin::OnRemovePolicy, FuncOperateType::REMOVE);
    ptr->SetOnAdminRemoveDoneListener(&DisallowedUninstallBundlesPlugin::OnAdminRemoveDone);
    SetAppInstallControlRuleType(AppExecFwk::AppInstallControlRuleType::DISALLOWED_UNINSTALL);
}

ErrCode DisallowedUninstallBundlesPlugin::OnGetPolicy(std::string &policyData, MessageParcel &data,
    MessageParcel &reply, int32_t userId)
{
    EDMLOGI("DisallowedUninstallBundlesPlugin OnGetPolicy policyData : %{public}s, userId : %{public}d",
        policyData.c_str(), userId);
    return GetBundlePolicy(policyData, data, reply, userId);
}
} // namespace EDM
} // namespace OHOS
