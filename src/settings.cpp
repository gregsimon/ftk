
#include "wx/wxprec.h"

#include "ftk.h"
#include "sdk.h"
#include "settings.h"
#include "ftk_platform.h"

#include "wx/filename.h"
#include "wx/stdpaths.h"
#include "wx/dir.h"

#include "../third_party/yaml/include/yaml.h"

namespace ftk 
{

  //
  // Currently we store settings in $HOME/.ftk/settings
  //

Settings::Settings(const wxString& path)
{
  wxStandardPaths& paths = wxStandardPaths::Get();
  
  wxFileName settings_path;
  settings_path.AssignDir(paths.GetUserConfigDir());
  settings_path.AppendDir(".ftk");

  if (!wxDir::Exists(settings_path.GetFullPath())) {
    if (!wxDir::Make(settings_path.GetFullPath(),wxS_DIR_DEFAULT)) {
      wxLogError("Unable to create settings folder %s", settings_path.GetFullPath());
    }
  }

  settings_path.SetName("settings");
  _settings_path = settings_path.GetFullPath();

  wxLogDebug("settings path -> %s", _settings_path);
  
  load();  // lload the settings file from disk

  currentSDK()->load(_sdk_path); // load the SDK

  int ss = (int)currentSDK()->packages().size();
  wxLogDebug("**** %d packages available", ss);
}

Settings::~Settings()
{
}

// Settings file is YAML
int Settings::load()
{
  // -- Load a new project
  yaml_parser_t parser;
  yaml_token_t  token;
  FILE *input;

  yaml_parser_initialize(&parser);

  if ((input = fopen(_settings_path, "rb")))
  {
    yaml_parser_set_input_file(&parser, input);
    int last_token_type = -1;
    wxString last_token_name;

    do {
      yaml_parser_scan(&parser, &token);
      switch (token.type)
      {
      case YAML_VALUE_TOKEN:
      case YAML_KEY_TOKEN:
        last_token_type = token.type;
        break;

      case YAML_SCALAR_TOKEN:
        if (last_token_type == YAML_KEY_TOKEN)
          last_token_name = (const char*)token.data.scalar.value;
        else {
          wxLogDebug("%s = %s",
            (const char*)last_token_name,
            (const char*)token.data.scalar.value);
          if (!last_token_name.Cmp("sdk_path"))
            _sdk_path = (const char*)token.data.scalar.value;
          else if (!last_token_name.Cmp("dart_path"))
            _dart_path = (const char*)token.data.scalar.value;
          last_token_name.clear();
        }
        break;
        default:
          wxLogDebug("Unhandled yaml token %d", token.type);
      }

      if (token.type != YAML_STREAM_END_TOKEN)
        yaml_token_delete(&token);

    } while (token.type != YAML_STREAM_END_TOKEN);


    fclose(input);
  }
  else {
    wxLogError("settings failed to open");
    return -1;
  }

  yaml_parser_delete(&parser);
  return -0;
}

int Settings::save()
{
  return -1;
}

} // namespace ftk



