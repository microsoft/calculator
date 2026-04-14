# Windows Calculator Troubleshooting Guide

This guide helps users solve common problems with Windows Calculator.

## Common Issues

### Calculator Won't Open

**Symptom:** Double-clicking Calculator does nothing or shows an error.

**Solutions:**
1. **Restart your computer** - Sometimes a simple reboot resolves loading issues.
2. **Check for updates** - Install the latest Windows updates from Settings > Windows Update.
3. **Reset Calculator:**
   - Go to Settings > Apps > Installed Apps
   - Find Calculator and click "Modify" or "Repair"
   - If that doesn't work, click "Uninstall" then reinstall from Microsoft Store.

### Display or Rendering Issues

**Symptom:** Calculator display is blank, distorted, or shows incorrect values.

**Solutions:**
1. **Restart Calculator** - Close and reopen the app.
2. **Update your display drivers** - Visit your graphics card manufacturer's website.
3. **Check scaling settings** - Right-click Calculator > Properties > Compatibility > Change high DPI settings.

### Currency Converter Shows Wrong Values

**Symptom:** Currency conversions seem incorrect or outdated.

**Solutions:**
1. Currency rates are updated periodically—check your internet connection.
2. Rates in developer builds use mock data (shows planetary names instead of currencies).
3. For accurate rates, use the Microsoft Store version of Calculator.

### Unit Converter Errors

**Symptom:** Unit conversions give unexpected results.

**Solutions:**
1. Verify you're selecting the correct unit types (e.g., temperature scale).
2. Check if you need to use the scientific mode for advanced conversions.
3. Some conversions are approximate—use for estimation only.

### Memory or History Not Working

**Symptom:** Memory (M+, M-, MR) buttons don't work or history is empty.

**Solutions:**
1. **Clear memory:** Click "MC" to clear stored memory values.
2. **Enable history:** Go to Settings > History and ensure it's turned on.
3. **Restart the app** to refresh the state.

### Performance Issues

**Symptom:** Calculator runs slowly or lags during calculations.

**Solutions:**
1. **Close other apps** to free up system resources.
2. **Check for background updates** - Pause Windows updates while using Calculator.
3. **Ensure sufficient disk space** - At least 100MB recommended.

### Keyboard Shortcuts Not Working

**Symptom:** Pressing shortcuts like `Alt+2` for scientific mode does nothing.

**Solutions:**
1. Calculator must be the active window for shortcuts to work.
2. Some shortcuts require Administrator mode.
3. Check keyboard language settings in Windows.

## Installation Issues

### Cannot Install from Microsoft Store

**Solutions:**
1. Sign out and back into Microsoft Store.
2. Clear Microsoft Store cache:
   - Press `Win+R`, type `wsreset.exe`, press Enter.
3. Check your date/time settings are correct.

### Builds from GitHub Don't Work

The GitHub version requires:
- Windows 11, build 22000 or newer
- Visual Studio 2022 with UWP workload
- Latest Windows 11 SDK

See [README.md](README.md) for full prerequisites.

## Getting More Help

- **Report bugs:** Use Feedback Hub or open an issue on GitHub
- **Feature requests:** Open an issue on GitHub with "feature" label
- **Contribute:** See [CONTRIBUTING.md](../CONTRIBUTING.md)

## Diagnostic Data

If issues persist, you may be asked to enable diagnostic data:
1. Open Calculator
2. Go to Settings > Privacy
3. Enable diagnostic data collection

This helps Microsoft investigate issues.