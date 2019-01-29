# New feature process

## When should I follow this process?
You need to follow this process for any change which "users will notice". This applies especially
to new features and major visual changes.

You do not need to follow this process for bug fixes, performance improvements, or changes to the
development tools. To contribute these changes, discuss the issue with the team and then submit a
pull request.

## Step 1: Create an issue and discuss with the community
New features are submitted in Feedback Hub. In Feedback Hub you can upvote existing feedback or
submit your own. We also encourage discussion on open issues in Feedback Hub and in GitHub.

## Step 2: Wait for Microsoft product team sponsorship
New features must have a sponsor from the Microsoft product team. We can only work on a few ideas
at a time, so some good feature ideas might remain open but unassigned to a sponsor.

## Step 3: Scoping and feature pitch
Once we've decided to sponsor a feature, a member of the Microsoft team will write a
*feature pitch*. The feature pitch concisely describes our point of view on the problem and will
typically include these sections:

* **Problem Statement**: What problem are we trying to solve? Who’s the customer? Is there a
  customer need or pain point we need to remedy? Is there a business goal or metric we are trying
  to improve? Do we have a hypothesis we want to prove or disprove?
* **Evidence or User Insights**: Why should we do this? Potential sources of data: Feedback Hub,
  GitHub, request from another team, telemetry data, anecdotes from listening to customers in
  person, user research, market or competitive research
* **Proposal**: How will the solution/feature help us solve the problem? How will the
  solution/feature meet the customer’s needs? How will the solution/feature improve the metrics?
  Who’s the target audience?
* **Risks**: This section may not be necessary if covered by the problem statement. What is the
  risk if we don’t do this work? What is the risk if we do?
* **Goals**: What you want to accomplish with this feature. Typical examples include
  “User Can *perform some task*”
* **Non-Goals**: Things we are explicitly not doing or supporting or that are out of scope,
  including any reasoning to why.

The feature pitch may also include a low-fidelity concept which will be refined during the
prototyping process.

We will edit the issue description on GitHub to include the feature pitch.

## Step 4: Prototyping
After the goals are written, we think of a variety of ways to address these goals and build
*prototypes* to try them out. We welcome community participation in this process.

Prototypes can take many forms. For many ideas, making changes directly to the app code (without
spending too much time making the code robust or maintainable) can be a fast and effective way to
try out new ideas. Or you might prefer to use design software, or even pencil and paper. Work from
low-fidelity to high-fidelity&mdash;try a few ideas for the overall approach before making your
preferred design pixel-perfect.

An important part of the prototyping process is sharing our work along the way, getting feedback,
and iterating on the design. Drawings, links to code, and other work-in-progress can be added to
the wiki for this project. Progress updates will be posted in the issues section.

During the investigation phase, we might discover that the idea isn't feasible or doesn't align
with our product roadmap. If this happens, we'll document what we learned and close the issue.

## Step 5: Prototype review
Once there is a high-fidelity design which addresses the goals described in the original pitch, the
Microsoft product team will review the prototype and ensure all items on this checklist are
addressed:

- [ ] Is there a high-fidelity design which gives reviewers a clear idea of how the feature will
  look and function when implemented?
- [ ] Has the plan been shared with the community (documented on the wiki and updates posted in the
  original issue) and have others been given an opportunity to provide suggestions?
- [ ] Are [Fluent design principles](https://docs.microsoft.com/en-us/windows/uwp/design/fluent-design-system/)
  followed? If we do something which deviates from the guidelines, do we have a good reason?
- [ ] Does the design include provisions for [all users](https://docs.microsoft.com/en-us/windows/uwp/design/accessibility/designing-inclusive-software)
  and [all cultures](https://docs.microsoft.com/en-us/windows/uwp/design/globalizing/guidelines-and-checklist-for-globalizing-your-app)?
- [ ] Is it technically feasible to build this feature? Take a look at the "before committing"
  checklist below and identify any issues which are likely to be blockers.

## Step 6: Implementation
A feature can be implemented by the original proposer, the Microsoft team sponsor, or by other
community members. Code contributions and testing help are greatly appreciated. Please let us know
in the issue comments if you're actively working on a feature so we can ensure it's assigned to
you.

You might be able to reuse code written during the prototype process, although there will typically
be more work required to make the solution robust. Once the code is ready, you can begin
[submitting pull requests](../CONTRIBUTING.md).

## Step 7: Technical review
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
    - [ ] All logging should use [TraceLogging](https://docs.microsoft.com/en-us/windows/desktop/tracelogging/trace-logging-about).
    - [ ] Unnecessary log events should be removed, or configured so that they are collected only when
      needed to debug issues or measure feature usage.
- [ ] If the change reads user data from files or app settings:
    - [ ] Verify that state saved in a previous version of the app can be used with the new version.
- [ ] If the change makes network requests:
    - [ ] Microsoft must plan to keep these dependencies secure and functional for the lifetime of
      the app (which might be several years).
    - [ ] The app should be fully functional if some network requests are slow or fail. Tools like
      [Fiddler](http://docs.telerik.com/fiddler/knowledgebase/fiddlerscript/perftesting)
      can be used to simulate slow or failed requests.

## Step 8: Final product review and merge to master
After the technical review is complete, the product team will review the finished product to make
sure the final implementation is ready to release to Windows customers.

## Step 9: Release
The release process is handled internally by the Microsoft team. When we release, we create a
`servicing` branch from master. We merge changes into servicing branches only to fix severe bugs.

Releases are distributed through the Microsoft Store, first to Windows Insiders and then to all
supported Windows 10 devices once we are confident in the update's quality. We usually ship an
update every month. After the app has been released to the Microsoft Store, it will be part of
the next major update to Windows 10 (especially for new devices).