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
#include "device_info_addon.h"
#include "edm_log.h"
#include "policy_info.h"

using namespace OHOS::EDM;

napi_value DeviceInfoAddon::Init(napi_env env, napi_value exports)
{
    napi_property_descriptor property[] = {
        DECLARE_NAPI_FUNCTION("getDeviceSerial", GetDeviceSerial),
        DECLARE_NAPI_FUNCTION("getDisplayVersion", GetDisplayVersion),
        DECLARE_NAPI_FUNCTION("getDeviceName", GetDeviceName),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(property) / sizeof(property[0]), property));
    return exports;
}

napi_value DeviceInfoAddon::GetDeviceSerial(napi_env env, napi_callback_info info)
{
    EDMLOGI("NAPI_GetDeviceSerial called");
    return GetDeviceInfo(env, info, GET_DEVICE_SERIAL);
}

napi_value DeviceInfoAddon::GetDisplayVersion(napi_env env, napi_callback_info info)
{
    EDMLOGI("NAPI_GetDisplayVersion called");
    return GetDeviceInfo(env, info, GET_DISPLAY_VERSION);
}

napi_value DeviceInfoAddon::GetDeviceName(napi_env env, napi_callback_info info)
{
    EDMLOGI("NAPI_GetDeviceName called");
    return GetDeviceInfo(env, info, GET_DEVICE_NAME);
}

napi_value DeviceInfoAddon::GetDeviceInfo(napi_env env, napi_callback_info info, int code)
{
    size_t argc = ARGS_SIZE_TWO;
    napi_value argv[ARGS_SIZE_TWO] = {nullptr};
    napi_value thisArg = nullptr;
    void *data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisArg, &data));
    ASSERT_AND_THROW_PARAM_ERROR(env, argc >= ARGS_SIZE_ONE, "parameter count error");
    bool matchFlag = MatchValueType(env, argv[ARR_INDEX_ZERO], napi_object);
    if (argc > ARGS_SIZE_ONE) {
        matchFlag = matchFlag && MatchValueType(env, argv[ARR_INDEX_ONE], napi_function);
    }
    ASSERT_AND_THROW_PARAM_ERROR(env, matchFlag, "parameter type error");
    auto asyncCallbackInfo = new (std::nothrow) AsyncGetDeviceInfoCallbackInfo();
    if (asyncCallbackInfo == nullptr) {
        return nullptr;
    }
    std::unique_ptr<AsyncGetDeviceInfoCallbackInfo> callbackPtr {asyncCallbackInfo};
    bool ret = ParseElementName(env, asyncCallbackInfo->elementName, argv[ARR_INDEX_ZERO]);
    ASSERT_AND_THROW_PARAM_ERROR(env, ret, "element name param error");
    EDMLOGD("GetDeviceInfo: asyncCallbackInfo->elementName.bundlename %{public}s, "
        "asyncCallbackInfo->abilityname:%{public}s",
        asyncCallbackInfo->elementName.GetBundleName().c_str(),
        asyncCallbackInfo->elementName.GetAbilityName().c_str());
    if (argc > ARGS_SIZE_ONE) {
        EDMLOGD("NAPI_GetDeviceInfo argc == ARGS_SIZE_TWO");
        napi_create_reference(env, argv[ARR_INDEX_ONE], NAPI_RETURN_ONE, &asyncCallbackInfo->callback);
    }
    asyncCallbackInfo->policyCode = code;
    napi_value asyncWorkReturn = HandleAsyncWork(env, asyncCallbackInfo, "GetDeviceInfo",
        NativeGetDeviceInfo, NativeStringCallbackComplete);
    callbackPtr.release();
    return asyncWorkReturn;
}

void DeviceInfoAddon::NativeGetDeviceInfo(napi_env env, void *data)
{
    EDMLOGI("NAPI_NativeGetDeviceInfo called");
    if (data == nullptr) {
        EDMLOGE("data is nullptr");
        return;
    }
    AsyncGetDeviceInfoCallbackInfo *asyncCallbackInfo = static_cast<AsyncGetDeviceInfoCallbackInfo *>(data);
    auto deviceInfoProxy_ = DeviceInfoProxy::GetDeviceInfoProxy();
    if (deviceInfoProxy_ == nullptr) {
        EDMLOGE("can not get DeviceInfoProxy");
        return;
    }
    asyncCallbackInfo->ret = deviceInfoProxy_->GetDeviceInfo(asyncCallbackInfo->elementName,
        asyncCallbackInfo->stringRet, asyncCallbackInfo->policyCode);
}

static napi_module g_deviceInfoModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = DeviceInfoAddon::Init,
    .nm_modname = "enterprise.deviceInfo",
    .nm_priv = ((void *)0),
    .reserved = { 0 },
};

extern "C" __attribute__((constructor)) void DeviceInfoRegister()
{
    napi_module_register(&g_deviceInfoModule);
}