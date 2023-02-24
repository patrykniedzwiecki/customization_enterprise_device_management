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

#include <gtest/gtest.h>
#include <string>
#include <system_ability_definition.h>
#include <vector>

#include "enterprise_device_mgr_stub_mock.h"
#include "edm_sys_manager_mock.h"
#include "network_manager_proxy.h"
#include "policy_info.h"

using namespace testing::ext;
using namespace testing;

namespace OHOS {
namespace EDM {
namespace TEST {
const std::string ADMIN_PACKAGENAME = "com.edm.test.demo";
class NetworkManagerProxyTest : public testing::Test {
protected:
    void SetUp() override;

    void TearDown() override;

    std::shared_ptr<NetworkManagerProxy> networkManagerProxy = nullptr;
    std::shared_ptr<EdmSysManager> edmSysManager_ = nullptr;
    sptr<EnterpriseDeviceMgrStubMock> object_ = nullptr;
};

void NetworkManagerProxyTest::SetUp()
{
    networkManagerProxy = NetworkManagerProxy::GetNetworkManagerProxy();
    edmSysManager_ = std::make_shared<EdmSysManager>();
    object_ = new (std::nothrow) EnterpriseDeviceMgrStubMock();
    edmSysManager_->RegisterSystemAbilityOfRemoteObject(ENTERPRISE_DEVICE_MANAGER_SA_ID, object_);
}

void NetworkManagerProxyTest::TearDown()
{
    networkManagerProxy.reset();
    edmSysManager_->UnregisterSystemAbilityOfRemoteObject(ENTERPRISE_DEVICE_MANAGER_SA_ID);
    object_ = nullptr;
}

/**
 * @tc.name: TestGetAllNetworkInterfacesSuc
 * @tc.desc: Test GetAllNetworkInterfaces func.
 * @tc.type: FUNC
 */
HWTEST_F(NetworkManagerProxyTest, TestGetAllNetworkInterfacesSuc, TestSize.Level1)
{
    AppExecFwk::ElementName admin;
    admin.SetBundleName(ADMIN_PACKAGENAME);
    EXPECT_CALL(*object_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(object_.GetRefPtr(), &EnterpriseDeviceMgrStubMock::InvokeArrayStringSendRequestGetPolicy));
    std::vector<std::string> networkInterfaces;
    int32_t ret = networkManagerProxy->GetAllNetworkInterfaces(admin, networkInterfaces);
    ASSERT_TRUE(ret == ERR_OK);
    ASSERT_TRUE(networkInterfaces.size() == 1);
    ASSERT_TRUE(networkInterfaces[0] == RETURN_STRING);
}

/**
 * @tc.name: TestGetAllNetworkInterfacesFail
 * @tc.desc: Test GetAllNetworkInterfaces func.
 * @tc.type: FUNC
 */
HWTEST_F(NetworkManagerProxyTest, TestGetAllNetworkInterfacesFail, TestSize.Level1)
{
    AppExecFwk::ElementName admin;
    admin.SetBundleName(ADMIN_PACKAGENAME);
    std::vector<std::string> networkInterfaces;
    int32_t ret = networkManagerProxy->GetAllNetworkInterfaces(admin, networkInterfaces);
    ASSERT_TRUE(ret != ERR_OK);
}

/**
 * @tc.name: TestGetIpOrMacAddressSuc
 * @tc.desc: Test GetIpOrMacAddress func.
 * @tc.type: FUNC
 */
HWTEST_F(NetworkManagerProxyTest, TestGetIpOrMacAddressSuc, TestSize.Level1)
{
    AppExecFwk::ElementName admin;
    admin.SetBundleName(ADMIN_PACKAGENAME);
    EXPECT_CALL(*object_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Invoke(object_.GetRefPtr(), &EnterpriseDeviceMgrStubMock::InvokeSendRequestGetPolicy));
    std::string info;
    int32_t ret = networkManagerProxy->GetIpOrMacAddress(admin, "eth0", GET_MAC, info);
    ASSERT_TRUE(ret == ERR_OK);
    ASSERT_TRUE(info == RETURN_STRING);
}

/**
 * @tc.name: TestGetIpOrMacAddressFail
 * @tc.desc: Test GetIpOrMacAddress func.
 * @tc.type: FUNC
 */
HWTEST_F(NetworkManagerProxyTest, TestGetIpOrMacAddressFail, TestSize.Level1)
{
    AppExecFwk::ElementName admin;
    admin.SetBundleName(ADMIN_PACKAGENAME);
    std::string info;
    int32_t ret = networkManagerProxy->GetIpOrMacAddress(admin, "eth0", GET_MAC, info);
    ASSERT_TRUE(ret != ERR_OK);
}
} // namespace TEST
} // namespace EDM
} // namespace OHOS
