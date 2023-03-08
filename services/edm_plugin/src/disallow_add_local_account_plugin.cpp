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

#include "disallow_add_local_account_plugin.h"
#include "long_serializer.h"
#include "os_account_manager.h"
#include "policy_info.h"

namespace OHOS {
namespace EDM {
const bool REGISTER_RESULT = IPluginManager::GetInstance()->AddPlugin(DisallowAddLocalAccountPlugin::GetPlugin());

void DisallowAddLocalAccountPlugin::InitPlugin(
    std::shared_ptr<IPluginTemplate<DisallowAddLocalAccountPlugin, bool>> ptr)
{
    EDMLOGD("DisallowAddLocalAccountPlugin InitPlugin...");
    std::string policyName;
    POLICY_CODE_TO_NAME(DISALLOW_ADD_LOCAL_ACCOUNT, policyName);
    ptr->InitAttribute(DISALLOW_ADD_LOCAL_ACCOUNT, policyName, "ohos.permission.ENTERPRISE_SET_ACCOUNT_POLICY",
        IPlugin::PermissionType::SUPER_DEVICE_ADMIN, true);
    ptr->SetSerializer(BoolSerializer::GetInstance());
    ptr->SetOnHandlePolicyListener(&DisallowAddLocalAccountPlugin::OnSetPolicy, FuncOperateType::SET);
    ptr->SetOnAdminRemoveListener(&DisallowAddLocalAccountPlugin::OnAdminRemove);
}

ErrCode DisallowAddLocalAccountPlugin::OnSetPolicy(bool &data)
{
    return SetGlobalOsAccountConstraints(data);
}

ErrCode DisallowAddLocalAccountPlugin::OnAdminRemove(const std::string &adminName, bool &data)
{
    return data ? SetGlobalOsAccountConstraints(!data) : ERR_OK;
}

ErrCode DisallowAddLocalAccountPlugin::SetGlobalOsAccountConstraints(bool data)
{
    std::vector<std::string> constraints = { "constraint.os.account.create.directly" };
    std::vector<int32_t> ids;
    AccountSA::OsAccountManager::QueryActiveOsAccountIds(ids);
    if (ids.empty()) {
        EDMLOGE("DisallowAddLocalAccountPlugin QueryActiveOsAccountIds failed");
        return EdmReturnErrCode::SYSTEM_ABNORMALLY;
    }
    ErrCode ret = AccountSA::OsAccountManager::SetGlobalOsAccountConstraints(constraints, data, ids.at(0), true);
    if (FAILED(ret)) {
        EDMLOGE("DisallowAddLocalAccountPlugin SetGlobalOsAccountConstraints failed");
        return EdmReturnErrCode::SYSTEM_ABNORMALLY;
    }
    return ERR_OK;
}
} // namespace EDM
} // namespace OHOS
