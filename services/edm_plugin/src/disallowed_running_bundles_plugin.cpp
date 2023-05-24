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

#include "disallowed_running_bundles_plugin.h"
#include <ipc_skeleton.h>
#include <system_ability_definition.h>

#include "app_control/app_control_proxy.h"
#include "array_string_serializer.h"
#include "edm_sys_manager.h"
#include "policy_info.h"

namespace OHOS {
namespace EDM {
const bool REGISTER_RESULT = IPluginManager::GetInstance()->AddPlugin(DisallowedRunningBundlesPlugin::GetPlugin());

constexpr int32_t MAX_SIZE = 200;

void DisallowedRunningBundlesPlugin::InitPlugin(std::shared_ptr<IPluginTemplate<DisallowedRunningBundlesPlugin,
    std::vector<std::string>>> ptr)
{
    EDMLOGD("DisallowedRunningBundlesPlugin InitPlugin...");
    std::string policyName;
    POLICY_CODE_TO_NAME(DISALLOW_RUNNING_BUNDLES, policyName);
    ptr->InitAttribute(DISALLOW_RUNNING_BUNDLES, policyName, "ohos.permission.ENTERPRISE_MANAGE_SET_APP_RUNNING_POLICY",
        IPlugin::PermissionType::SUPER_DEVICE_ADMIN, true);
    ptr->SetSerializer(ArrayStringSerializer::GetInstance());
    ptr->SetOnHandlePolicyListener(&DisallowedRunningBundlesPlugin::OnSetPolicy, FuncOperateType::SET);
    ptr->SetOnHandlePolicyListener(&DisallowedRunningBundlesPlugin::OnRemovePolicy, FuncOperateType::REMOVE);
    ptr->SetOnAdminRemoveDoneListener(&DisallowedRunningBundlesPlugin::OnAdminRemoveDone);
}

ErrCode DisallowedRunningBundlesPlugin::OnSetPolicy(std::vector<std::string> &data,
    std::vector<std::string> &currentData, int32_t userId)
{
    EDMLOGI("DisallowedRunningBundlesPlugin OnSetPolicy userId = %{public}d", userId);
    if (data.empty()) {
        EDMLOGW("DisallowedRunningBundlesPlugin OnSetPolicy data is empty:");
        return ERR_OK;
    }

    std::vector<std::string> mergeData = ArrayStringSerializer::GetInstance()->SetUnionPolicyData(data, currentData);
    if (mergeData.size() > MAX_SIZE) {
        EDMLOGE("DisallowedRunningBundlesPlugin OnSetPolicy data is too large:");
        return EdmReturnErrCode::PARAM_ERROR;
    }

    std::vector<AppExecFwk::AppRunningControlRule> controlRules;
    std::for_each(data.begin(), data.end(), [&](const std::string &str) {
        AppExecFwk::AppRunningControlRule controlRule;
        controlRule.appId = str;
        controlRules.push_back(controlRule);
    });

    ErrCode res = GetAppControlProxy()->AddAppRunningControlRule(controlRules, userId);
    if (res != ERR_OK) {
        EDMLOGE("DisallowedRunningBundlesPlugin OnSetPolicyDone Faild %{public}d:", res);
        return EdmReturnErrCode::SYSTEM_ABNORMALLY;
    }
    currentData = mergeData;
    return ERR_OK;
}

ErrCode DisallowedRunningBundlesPlugin::OnGetPolicy(std::string &policyData, MessageParcel &data, MessageParcel &reply,
    int32_t userId)
{
    EDMLOGI("DisallowedRunningBundlesPlugin OnGetPolicy policyData : %{public}s, userId : %{public}d",
        policyData.c_str(), userId);
    std::vector<std::string> appIds;
    pluginInstance_->serializer_->Deserialize(policyData, appIds);
    reply.WriteInt32(ERR_OK);
    reply.WriteInt32(appIds.size());
    reply.WriteStringVector(appIds);
    return ERR_OK;
}

ErrCode DisallowedRunningBundlesPlugin::OnRemovePolicy(std::vector<std::string> &data,
    std::vector<std::string> &currentData, int32_t userId)
{
    EDMLOGD("DisallowedRunningBundlesPlugin OnRemovePolicy userId : %{public}d:", userId);
    if (data.empty()) {
        EDMLOGW("DisallowedRunningBundlesPlugin OnRemovePolicy data is empty:");
        return ERR_OK;
    }
    std::vector<std::string> mergeData = ArrayStringSerializer::GetInstance()->
        SetDifferencePolicyData(data, currentData);
    std::vector<AppExecFwk::AppRunningControlRule> controlRules;
    std::for_each(data.begin(), data.end(), [&](const std::string &str) {
        AppExecFwk::AppRunningControlRule controlRule;
        controlRule.appId = str;
        controlRules.push_back(controlRule);
    });
    ErrCode res = GetAppControlProxy()->DeleteAppRunningControlRule(controlRules, userId);
    if (res != ERR_OK) {
        EDMLOGE("DisallowedRunningBundlesPlugin DeleteAppInstallControlRule OnRemovePolicy faild %{public}d:", res);
        return EdmReturnErrCode::SYSTEM_ABNORMALLY;
    }
    currentData = mergeData;
    return ERR_OK;
}

sptr<AppExecFwk::IAppControlMgr> DisallowedRunningBundlesPlugin::GetAppControlProxy()
{
    auto remoteObject = EdmSysManager::GetRemoteObjectOfSystemAbility(OHOS::BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    sptr<AppExecFwk::IBundleMgr> proxy = iface_cast<AppExecFwk::IBundleMgr>(remoteObject);
    return proxy->GetAppControlProxy();
}

void DisallowedRunningBundlesPlugin::OnAdminRemoveDone(const std::string &adminName, std::vector<std::string> &data,
    int32_t userId)
{
    EDMLOGI("DisallowedRunningBundlesPlugin OnAdminRemoveDone adminName : %{public}s userId : %{public}d",
        adminName.c_str(), userId);
    std::vector<AppExecFwk::AppRunningControlRule> controlRules;
    std::for_each(data.begin(), data.end(), [&](const std::string &str) {
        AppExecFwk::AppRunningControlRule controlRule;
        controlRule.appId = str;
        controlRules.push_back(controlRule);
    });
    ErrCode res = GetAppControlProxy()->DeleteAppRunningControlRule(controlRules, userId);
    EDMLOGI("DisallowedRunningBundlesPlugin OnAdminRemoveDone result %{public}d:", res);
}
} // namespace EDM
} // namespace OHOS