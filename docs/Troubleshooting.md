# Troubleshooting Guide

This guide helps users solve common problems with Windows Calculator.

--------------------

## Table of Contents

* [Common Error Messages](#common-error-messages)
* [Installation Issues](#installation-issues)
* [Configuration Problems](#configuration-problems)
* [Performance Issues](#performance-issues)

--------------------

## Common Error Messages

### Calculator won't start

**Symptoms:** Calculator app fails to launch or closes immediately after opening.

**Solutions:**
1. **Restart the app:** Close Calculator completely (right-click the taskbar icon → End task) and reopen it.
2. **Restart your PC:** A simple reboot can resolve temporary issues.
3. **Check for Windows updates:** Go to Settings → Windows Update and install any pending updates.
4. **Reset the app:** Go to Settings → Apps → Installed apps, find Calculator, click Advanced options → Reset.

### Calculation results appear incorrect

**Symptoms:** Mathematical operations produce unexpected results.

**Solutions:**
1. **Verify the input mode:** Ensure you have selected the correct mode (Standard, Scientific, Programmer, or Date).
2. **Check for operator precedence:** Remember that Calculator follows standard mathematical order of operations in Scientific mode.
3. **Clear and retry:** Press the "C" button to clear the current calculation and re-enter your expression.

### Memory functions not working

**Symptoms:** MS, MR, M+, or MC buttons don't behave as expected.

**Solutions:**
1. **Ensure a value is stored:** Press "MS" to store a number in memory before using MR.
2. **Clear memory if needed:** Press "MC" to clear the memory register.
3. **Check the display:** A "M" indicator should appear when a value is stored in memory.

--------------------

## Installation Issues

### Calculator is missing from Start menu

**Symptoms:** Cannot find Calculator in the Windows Start menu or app list.

**Solutions:**
1. **Search for Calculator:** Type "Calculator" in the Windows search bar.
2. **Reinstall via Microsoft Store:** If Calculator was removed, go to the Microsoft Store and reinstall it.
3. **Check Windows features:** Ensure the Calculator app is not disabled in Windows Features (Control Panel → Programs → Turn Windows features on or off).

### Updates fail to install

**Symptoms:** Calculator update errors or stuck update processes.

**Solutions:**
1. **Check your internet connection:** Ensure you have a stable internet connection.
2. **Clear the Microsoft Store cache:** Run `wsreset.exe` from the Run dialog (Win + R).
3. **Sign out of Microsoft Store:** Go to Microsoft Store → Profile → Sign out, then sign back in.

--------------------

## Configuration Problems

### Display shows wrong number format

**Symptoms:** Numbers appear in an unexpected format (e.g., commas in wrong places).

**Solutions:**
1. **Check Windows region settings:** Go to Settings → Time & Language → Language & region and verify your region settings.
2. **Adjust number format:** In Windows Control Panel → Region → Additional settings → Numbers to customize decimal and grouping symbols.

### Mode changes don't persist

**Symptoms:** Calculator resets to Standard mode each time it opens.

**Solutions:**
1. **Update Windows Calculator:** Ensure you have the latest version from the Microsoft Store.
2. **Check for known issues:** Search for similar reports in the issue tracker.

### Keyboard shortcuts not working

**Symptoms:** Pressing calculator keys on your keyboard has no effect.

**Solutions:**
1. **Ensure Calculator is focused:** Click on the Calculator window to make it the active app.
2. **Check keyboard language:** Ensure your keyboard language matches your system settings.

--------------------

## Performance Issues

### Calculator runs slowly

**Symptoms:** App feels sluggish or takes time to respond to inputs.

**Solutions:**
1. **Close other apps:** Reduce system load by closing unnecessary applications.
2. **Check for background processes:** Use Task Manager to identify resource-heavy processes.
3. **Update graphics drivers:** Outdated graphics drivers can affect app performance.

### High memory usage

**Symptoms:** Calculator uses more RAM than expected.

**Solutions:**
1. **Restart Calculator:** Close and reopen the app to clear memory.
2. **Check for memory leaks:** If the issue persists, restart your PC.
3. **Update the app:** Newer versions may have memory optimizations.

### Display rendering issues

**Symptoms:** Numbers or symbols appear blurry, especially on non-retina displays.

**Solutions:**
1. **Adjust ClearType:** Run the ClearType Text Tuner (search for it in Windows) to fine-tune font rendering.
2. **Check display scaling:** Go to Settings → Display → Scale and ensure it's set to recommended levels.
3. **Update graphics drivers:** Ensure your display adapter drivers are up to date.

### Audio feedback not working (if applicable)

**Symptoms:** Key press sounds are missing or incorrect.

**Solutions:**
1. **Check system sound settings:** Ensure system sounds are enabled in Windows settings.
2. **Check app sound settings:** If Calculator has sound options, verify they are enabled.
3. **Test with other apps:** Confirm your system's sound is working with other applications.

--------------------

## Getting Additional Help

If you continue to experience issues:

1. **Search existing issues:** Check if others have reported the same problem.
2. **Report a new issue:** Include your Windows version, Calculator version, and steps to reproduce.
3. **Check the documentation:** Refer to the [Application Architecture][ApplicationArchitecture.md] document for technical details.

[ApplicationArchitecture.md]: ./ApplicationArchitecture.md
