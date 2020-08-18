# 计算器
Windows计算器应用程序是一个用C++编写的现代Windows应用程序，预装在Windows中。
该应用程序提供了标准的、科学的和程序员的计算器功能，以及一套各种计量单位和货币之间的转换器。

计算器定期发布新功能和错误修复。你可以在[微软商店](https://www.microsoft.com/store/apps/9WZDNCRFHVN5)中获得最新版本的计算器。

[！[Build Status](https://dev.azure.com/ms/calculator/_apis/build/status/Calculator-CI?branchName=master)](https://dev.azure.com/ms/calculator/_build/latest?definitionId=57&branchName=master)

  !"计算器截图"(docs/Images/CalculatorScreenshot.png)

## 特点
- 标准计算器功能，提供基本的操作，并在输入命令时立即进行评估。
- 科学计算器功能，它提供了扩展的操作，并使用操作顺序来评估命令。
- 程序员计算器功能，为开发人员提供常见的数学运算，包括常见基础之间的转换。
- 日期计算功能，它提供了两个日期之间的差异，以及对给定的输入日期进行年、月、日的加减运算。
- 计算历史和记忆功能。
- 多种计量单位之间的转换。
- 基于从[Bing](https://www.bing.com)获取的数据的货币转换。
- 基本的[无限精度](https://en.wikipedia.org/wiki/Arbitrary-precision_arithmetic)。
  算术运算(加减乘除)，以便计算
  永远不会失去精度。

## ＃＃开始
前提条件。
- 你的电脑必须运行Windows 10，1803版本或更新版本。
- 安装最新版本的[Visual Studio](https://developer.microsoft.com/en-us/windows/downloads)（免费社区版即可）。
  - 安装 "通用Windows平台开发 "工作负载。
  - 安装可选的 "C++通用Windows平台工具 "组件。
  - 安装最新的Windows 10 SDK。

  ![Visual Studio安装截图](docs/Images/VSInstallationScreenshot.png)
- 安装[XAML Styler](https://marketplace.visualstudio.com/items?itemName=TeamXavalon.XAMLStyler) Visual Studio扩展。

- 获取代码。
    ```
    git clone https://github.com/Microsoft/calculator.git
    ```

- 在Visual Studio中打开[src/Calculator.sln](/src/Calculator.sln)，构建并运行Calculator应用。
- 关于计算器项目架构的一般描述，请参见[ApplicationArchitecture.md](docs/ApplicationArchitecture.md)。
- 要运行UI测试，你需要确保以下几点
  [Windows应用程序驱动程序（WinAppDriver）](https://github.com/microsoft/WinAppDriver/releases/latest)
  是安装。

## Contributing
想做出贡献？团队鼓励社区的反馈和贡献，请遵循我们的[贡献指南](CONTRIBUT.md)。请遵循我们的[贡献指南](CONTRIBUTING.md)。

如果计算器不能正常工作，请在[反馈枢纽](https://insider.windows.com/en-us/fb/?contextid=130)中提交报告，我们也欢迎[在GitHub上提交问题](https://insider.windows.com/en-us/fb/?contextid=130)。
我们也欢迎[在GitHub上提交问题](https://github.com/Microsoft/calculator/issues)。

## 路线图
有关Windows计算器计划和发布时间表的信息，请参见[Windows计算器路线图](docs/Roadmap.md)。

###图形模式
添加图形计算器功能[在项目路线图上](https://github.com/Microsoft/calculator/issues/338)，我们希望这个项目能够围绕图形化创造一个很好的终端用户体验。为此，来自官方收件箱中的Windows计算器的用户界面目前是这个资源库的一部分，尽管专有的微软构建的图形引擎，也驱动了微软数学和OneNote中的图形，但不是。社区成员仍然可以参与到UI的创建中来，然而开发者构建的UI将不具备图形功能，因为使用了构建在一个[引擎的模拟实现](/src/GraphingImpl/Mocks)之上的
通用图形API](/src/GraphingInterfaces)。

## 诊断数据
该项目收集使用数据并将其发送给 Microsoft，以帮助改进我们的产品和服务。
阅读我们的[隐私声明](https://go.microsoft.com/fwlink/?LinkId=521839)以了解更多信息。
诊断数据在开发版本中默认是被禁用的，可以通过`SEND_DIAGNOSTICS`来启用。
建立标志。

## ＃＃货币转换器
Windows计算器包括一个货币转换功能，在开发者构建中使用模拟数据。该数据
微软使用的货币转换功能（例如，在应用程序的零售版本）没有得到授权。
供您使用。模拟数据将可以清楚地识别，因为它引用的是行星而不是国家。
并且无论选择何种输入都保持静态。

##报告安全问题
请参考[SECURITY.md](./SECURITY.md)。

## 许可证
Copyright (c) Microsoft Corporation. 保留所有权利。

授權條款 

通过www.DeepL.com/Translator（免费版）翻译