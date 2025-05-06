# Sven Co-op Plugins Editor

This "pet project" program is developed using C++Builder, Delphi, and LLM. Honestly, I’m not a programmer, so please forgive any quality issues in the code. :D

It is designed to edit the `default_plugins.txt` file located in the `Sven Co-op\svencoop` directory.  
On the first run, the program will automatically search for the Sven Co-op folder in the Windows registry. If it succeeds (and has the necessary permissions), all the settings paths will be filled in automatically. Nevertheless, it is recommended to verify that everything was correctly detected or to adjust the paths manually in the Settings.

## How to Use

- **Adding Plugins:**  
  Double-clicking a file in the file tree will add the plugin to the list, but only if neither the relative path nor the name matches any existing entry. At any time, you can edit the path (e.g., specify "../maps").
  
- **Context Menu:**  
  Right-clicking in the plugins table opens a context menu that allows you to add or delete a plugin.

- **Saving and Exporting:**  
  For **Saving** (CTRL+S) and **Exporting** (CTRL+E), the mandatory fields are **Name** and **Script**; **Concommands** and **Admin Level** remain optional.  
  If the default profile is selected, you will be immediately prompted to export the data into the file `default_plugins.txt`.

- **Default Profile Restrictions:**  
  The default profile cannot be deleted for obvious reasons, as it is an important file not only for the program but also for Sven Co-op.

- **Profile Management:**  
  In the current version, switching to any other profile does not automatically save the state of the previous one (for example, when the focus is on the profiles field, it is easy to scroll it using the mouse wheel). Make sure to save profiles manually before switching!

## Profiles

You can either work directly with the file (the so-called default profile) or create copies referred to as Profiles.  
A Profile is simply a `.json` file located in the `profiles` directory (by default) and has a structure similar to `default_plugins.txt`, with the exception of the new `enabled` field, which affects the export of the profile to `default_plugins.txt`. In other words, when you export a profile, only the plugins that have been marked (enabled) will be saved.  
When exporting the default profile, any plugins that are unmarked will immediately disappear, as the `default_plugins.txt` file does not support this parameter.  
In Settings, you can change the parameter responsible for creating the default profile.

---

**Note:**  
This is the first public release of the program. It is still incomplete and not fully ready for production.

---
License: Creative Commons Attribution-NonCommercial 4.0 International