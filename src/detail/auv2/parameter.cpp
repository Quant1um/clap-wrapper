
#include "parameter.h"

namespace Clap::AUv2
{

Parameter::Parameter(const clap_plugin_t* plugin, const clap_plugin_params_t* clap_param_ext,
                     const clap_param_info_t& clap_param, const uint32_t auv2_id)
{
  _info = clap_param;
  _cfstring = CFStringCreateWithCString(NULL, _info.name, kCFStringEncodingUTF8);
  _auv2_id = auv2_id;
  _flags = 0;

  _flags |= kAudioUnitParameterFlag_Global;

  if (!(_info.flags & CLAP_PARAM_IS_AUTOMATABLE)) _flags |= kAudioUnitParameterFlag_NonRealTime;
  if (!(_info.flags & CLAP_PARAM_IS_HIDDEN))
  {
    if (_info.flags & CLAP_PARAM_IS_READONLY)
      _flags |= kAudioUnitParameterFlag_IsReadable;
    else
      _flags |= kAudioUnitParameterFlag_IsReadable | kAudioUnitParameterFlag_IsWritable;
  }
  if (_info.flags & CLAP_PARAM_IS_STEPPED)
  {
    if (_info.max_value - _info.min_value == 1) _flags |= kAudioUnitParameterUnit_Boolean;
    if (_info.flags & CLAP_PARAM_IS_ENUM) _flags |= kAudioUnitParameterUnit_Indexed;
  }
  else
  {
    _flags |= kAudioUnitParameterFlag_IsHighResolution;
  }

  // checking if the parameter supports the conversion of its value to text
  // we can't get the value since we are not in the audio thread
  // auto guarantee_mainthread = _plugin->AlwaysMainThread();
  {
    char buf[200];
    if (clap_param_ext->value_to_text(plugin, _info.id, _info.default_value, buf, sizeof(buf)))
    {
      _flags |= kAudioUnitParameterFlag_HasName;
    }
  }

  /*
   * The CFString() used from the param can reset which releases it. So add a ref count
   * and ask the param to release it too
   */
  _flags |= kAudioUnitParameterFlag_HasCFNameString | kAudioUnitParameterFlag_CFNameRelease;
}

Parameter::~Parameter()
{
  CFRelease(_cfstring);
}

}  // namespace Clap::AUv2
