/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include "enterprise_admin_conn_manager.h"

#include <if_system_ability_manager.h>
#include <ipc_skeleton.h>
#include <iservice_registry.h>
#include <string_ex.h>
#include <system_ability_definition.h>

#include "ability_manager_client.h"
#include "edm_log.h"

using namespace OHOS::AAFwk;

namespace OHOS {
namespace EDM {
bool EnterpriseAdminConnManager::ConnectAbility(const std::string& bundleName, const std::string& abilityName,
    uint32_t code, int32_t userId)
{
    EDMLOGI("enter, target bundle = %{public}s", bundleName.c_str());
    std::lock_guard<std::mutex> lock(mutex_);

    if (!GetAbilityMgrProxy()) {
        EDMLOGE("failed to get ability manager proxy!");
        return -1;
    }

    sptr<EnterpriseAdminConnection> connection(new (std::nothrow) EnterpriseAdminConnection(code));
    Want want;
    want.SetElementName(bundleName, abilityName);
    int32_t ret = abilityMgr_->ConnectAbility(want, connection, nullptr, userId);
    EDMLOGI("ConnectAbility over.");
    if (ret != ERR_OK) {
        EDMLOGE("connect failed");
        return false;
    }
    return true;
}

bool EnterpriseAdminConnManager::GetAbilityMgrProxy()
{
    EDMLOGI("GetAbilityMgrProxy enter");
    if (abilityMgr_ == nullptr) {
        sptr<ISystemAbilityManager> systemAbilityManager =
            SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (systemAbilityManager == nullptr) {
            EDMLOGE("Failed to get system ability mgr.");
            return false;
        }

        sptr<IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(ABILITY_MGR_SERVICE_ID);
        if (remoteObject == nullptr) {
            EDMLOGE("Failed to get ability manager service.");
            return false;
        }

        abilityMgr_ = iface_cast<AAFwk::IAbilityManager>(remoteObject);
        if ((abilityMgr_ == nullptr) || (abilityMgr_->AsObject() == nullptr)) {
            EDMLOGE("Failed to get system ability manager services ability");
            return false;
        }

        deathRecipient_ = (new (std::nothrow) AbilityManagerDeathRecipient());
        if (deathRecipient_ == nullptr) {
            EDMLOGE("Failed to create AbilityManagerDeathRecipient");
            return false;
        }
        if (!abilityMgr_->AsObject()->AddDeathRecipient(deathRecipient_)) {
            EDMLOGW("Failed to add AbilityManagerDeathRecipient");
        }
    }
    return true;
}

void EnterpriseAdminConnManager::Clear()
{
    EDMLOGI("enter");
    std::lock_guard<std::mutex> lock(mutex_);

    if ((abilityMgr_ != nullptr) && (abilityMgr_->AsObject() != nullptr)) {
        abilityMgr_->AsObject()->RemoveDeathRecipient(deathRecipient_);
    }
    abilityMgr_ = nullptr;
}
} // namespace EDM
} // namespace OHOS
