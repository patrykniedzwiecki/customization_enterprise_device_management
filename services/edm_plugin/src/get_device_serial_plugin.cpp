/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include "get_device_serial_plugin.h"
#include "string_serializer.h"
#include "parameter.h"
#include "policy_info.h"

namespace OHOS {
namespace EDM {
const bool REGISTER_RESULT = IPluginManager::GetInstance()->AddPlugin(GetDeviceSerialPlugin::GetPlugin());

void GetDeviceSerialPlugin::InitPlugin(std::shared_ptr<IPluginTemplate<GetDeviceSerialPlugin, std::string>> ptr)
{
    EDMLOGD("GetDeviceSerialPlugin InitPlugin...");
    std::string policyName;
    POLICY_CODE_TO_NAME(GET_DEVICE_SERIAL, policyName);
    ptr->InitAttribute(GET_DEVICE_SERIAL, policyName, "ohos.permission.ENTERPRISE_GET_DEVICE_INFO",
        IPlugin::PermissionType::SUPER_DEVICE_ADMIN, false);
    ptr->SetSerializer(StringSerializer::GetInstance());
}

ErrCode GetDeviceSerialPlugin::OnGetPolicy(std::string &policyData, MessageParcel &data, MessageParcel &reply,
    int32_t userId)
{
    EDMLOGI("GetDeviceSerialPlugin OnGetPolicy.");
    std::string serial = GetSerial();
    reply.WriteInt32(ERR_OK);
    reply.WriteString(serial);
    return ERR_OK;
}
} // namespace EDM
} // namespace OHOS