# New feature process

## Where do I submit my idea for a new feature?
The easiest way to submit new feature requests is through 
[Feedback Hub](https://insider.windows.com/en-us/fb/?contextid=130).
In Feedback Hub, any Windows user (even if they aren't on GitHub) can upvote suggestions. The
Calculator team reviews these suggestions regularly, and when we're ready to work on an idea we
create [feature pitch issues here on GitHub](https://github.com/Microsoft/calculator/issues?q=is%3Aissue+is%3Aopen+project%3AMicrosoft%2Fcalculator%2F1).

But using Feedback Hub is not required&mdash;_you_ can create feature pitches too! This document
explains the elements of a good pitch and how we'll guide features from a pitch to a finished
product. The [Feature Tracking board](https://github.com/Microsoft/calculator/projects/1) shows
all the features we're working on and where they're at in the process.

## Do I need to follow this process? Can I just start coding and submit a PR?
You **do not** need to follow this process for bug fixes, performance improvements, or changes to the
development tools. To contribute these changes, 
[discuss the proposed change in an issue](https://github.com/Microsoft/calculator/issues/new)
and then submit a pull request.

You **do** need to follow this process for any change which "users will notice". This applies
especially to new features and major visual changes.

## Step 1: Feature pitch
Feature pitches are submitted as issues on GitHub using the 
[Feature Request template](https://github.com/Microsoft/calculator/issues/new?assignees=&labels=&template=feature_request.md&title=). 
We encourage discussion on open issues, and as discussion progresses we will edit the issue description to refine the 
idea until it is ready for review.

We review pitches regularly, and will approve or close issues based on whether they broadly align with the 
[Calculator roadmap](https://github.com/Microsoft/calculator/blob/master/docs/Roadmap.md). Approved pitches are moved 
into [planning](https://github.com/Microsoft/calculator/projects/1) on the feature tracking board.

## Step 2: Planning
For most features, the output of this phase is a specification which describes how the feature will work, supported by 
design renderings and code prototypes as needed. The original issue will continue to track the overall progress of the 
feature, but we will create and iterate on spec documentation in the 
[Calculator Spec repo](https://github.com/Microsoft/calculator-specs). Sometimes we'll learn new things about a feature 
proposal during planning, and we'll edit or close the original pitch.

We welcome community participation throughout planning. The best ideas often come from trying many ideas during 
the planning phase. To enable rapid
experimentation, we encourage developing and sharing rough ideas&mdash;maybe even with pencil and
paper&mdash;before making designs pixel-perfect or making code robust and maintainable.

After the [spec review](https://github.com/Microsoft/calculator-specs#spec-review) is completed, we will move the issue 
into [implementation](https://github.com/Microsoft/calculator/projects/1) on the feature tracking board. In _some_ cases, 
all of the details of an idea can be captured concisely in original feature pitch. When that happens, we may move ideas
directly into implementation.

## Step 3: Implementation
A feature can be implemented by the original submitter, a Microsoft team member, or by other
community members. Code contributions and testing help are greatly appreciated. Please let everyone know if you're 
actively working on a feature to help avoid duplicated efforts from others.

You might be able to reuse code written during the prototype process, although there will typically
be more work required to make the solution robust. Once the code is ready, you can begin
[submitting pull requests](../CONTRIBUTING.md).

### Technical review
As with all changes, the code for new features will be reviewed by a member of the Microsoft team
before being checked in to the master branch.

New features often need a more thorough technical review than bug fixes. When reviewing code for
new features, the Microsoft team considers at least these items:

- [ ] All items on the [Accessibility checklist](https://docs.microsoft.com/en-us/windows/uwp/design/accessibility/accessibility-checklist)
  should be addressed.
- [ ] All items on the [Globalization checklist](https://docs.microsoft.com/en-us/windows/uwp/design/globalizing/guidelines-and-checklist-for-globalizing-your-app)
  should be addressed.
- [ ] The change should be tested on the oldest version of Windows that the app supports. You can
  find this version number in AppxManifest.xml. Any calls to APIs newer than that version should be
  [conditionally enabled](https://docs.microsoft.com/en-us/windows/uwp/debug-test-perf/version-adaptive-apps).
- [ ] The change should use only supported APIs. If there are any questions about whether legacy or
  undocumented APIs are in use, the [Windows App Certification Kit](https://docs.microsoft.com/en-us/windows/uwp/debug-test-perf/windows-app-certification-kit)
  should be run to check.
- [ ] The change should save the user's progress if the app is
  [suspended and resumed](https://docs.microsoft.com/en-us/windows/uwp/debug-test-perf/optimize-suspend-resume).
  Code to handle these cases should be
  [tested in the Visual Studio debugger](https://docs.microsoft.com/en-us/visualstudio/debugger/how-to-trigger-suspend-resume-and-background-events-for-windows-store-apps-in-visual-studio).
- [ ] If the change [has customizations for particular device families](https://docs.microsoft.com/en-us/uwp/extension-sdks/device-families-overview),
  it should be tested on those device families.
- [ ] The change should be tested with the app window resized to the smallest possible size.
- [ ] The change should be tested with light, dark, and high contrast themes. It should honor the
  user's preferred [accent color](https://docs.microsoft.com/en-us/windows/uwp/design/style/color#accent-color-palette).
- [ ] If the change adds new libraries or other dependencies:
    - [ ] If the library is packaged with the app, the increased size of the binaries should be
      measured.
    - [ ] If the library is not maintained by Microsoft, the Microsoft team will need to set up a
      plan to monitor the upstream library for changes like security fixes.
    - [ ] If the library is being used under an open-source license, we must comply with the license
      and credit third parties appropriately.
- [ ] If the change adds code which runs during the app's startup path, or adds new XAML elements
  which are loaded at startup:
    - [ ] Run the perf tests to measure any increase in startup time. Move work out of the startup
      path if possible.
- [ ] If the change adds additional logging:
    - [ ] All logging should use 
         [TraceLogging](https://docs.microsoft.com/en-us/windows/desktop/tracelogging/trace-logging-about).
    - [ ] Unnecessary log events should be removed, or configured so that they are collected only when
      needed to debug issues or measure feature usage.
- [ ] If the change reads user data from files or app settings:
    - [ ] Verify that state saved in a previous version of the app can be used with the new version.
- [ ] If the change makes network requests:
    - [ ] Microsoft must plan to keep these dependencies secure and functional for the lifetime of
      the app (which might be several years).
    - [ ] The app should be fully functional if some network requests are slow or fail. Tools like
      [Fiddler](https://docs.telerik.com/fiddler/knowledgebase/fiddlerscript/perftesting)
      can be used to simulate slow or failed requests.

## Step 4: Final product review and merge to master
After the technical review is complete, the product team will review the finished product to make
sure the final implementation is ready to [release to Windows customers](Roadmap.md#Releases).
