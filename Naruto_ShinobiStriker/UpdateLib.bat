

xcopy	/y		.\Engine\Bin\Engine.lib					.\EngineSDK\Lib\
xcopy	/y		.\Engine\ThirdPartyLib\*					.\EngineSDK\Lib\
xcopy	/y		.\Engine\Bin\Engine.dll					.\Client\Bin\
xcopy	/y/s		.\Engine\Public\*.*					.\EngineSDK\Inc\
xcopy	/y		.\Engine\Bin\ShaderFiles\*.*			.\Client\Bin\ShaderFiles\