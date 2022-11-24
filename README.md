# Calculator
The Windows Calculator app is a modern Windows app written in C++ and C# that ships pre-installed with Windows.
The app provides standard, scientific, and programmer calculator functionality, as well as a set of converters between various units of measurement and currencies.

Calculator ships regularly with new features and bug fixes. You can get the latest version of Calculator in the [Microsoft Store](https://www.microsoft.com/store/apps/9WZDNCRFHVN5).

O aplicativo Calculadora do Windows é um aplicativo moderno do Windows escrito em C++ e C# que vem pré-instalado com o Windows.
O aplicativo fornece funcionalidade de calculadora padrão, científica e de programador, bem como um conjunto de conversores entre várias unidades de medida e moedas.

A calculadora é enviada regularmente com novos recursos e correções de bugs. Você pode obter a versão mais recente da Calculadora na [Microsoft Store](https://www.microsoft.com/store/apps/9WZDNCRFHVN5).

[![Build Status](https://dev.azure.com/ms/calculator/_apis/build/status/Calculator-CI?branchName=main)](https://dev.azure.com/ms/calculator/_build/latest?definitionId=57&branchName=main)

  ![Calculator Screenshot](docs/Images/CalculatorScreenshot.png)

## Features
- Standard Calculator functionality which offers basic operations and evaluates commands immediately as they are entered.
- Scientific Calculator functionality which offers expanded operations and evaluates commands using order of operations.
- Programmer Calculator functionality which offers common mathematical operations for developers including conversion between common bases.
- Date Calculation functionality which offers the difference between two dates, as well as the ability to add/subtract years, months and/or days to/from a given input date.
- Funcionalidade de calculadora padrão que oferece operações básicas e avalia comandos imediatamente à medida que são inseridos.
- Funcionalidade da Calculadora Científica que oferece operações expandidas e avalia comandos usando a ordem das operações.
- Funcionalidade da calculadora do programador que oferece operações matemáticas comuns para desenvolvedores, incluindo conversão entre bases comuns.
- Funcionalidade de cálculo de data que oferece a diferença entre duas datas, bem como a capacidade de adicionar/subtrair anos, meses e/ou dias a/de uma determinada data de entrada.
- Calculation history and memory capabilities.
- Conversion between many units of measurement.
- Currency conversion based on data retrieved from [Bing](https://www.bing.com).
- [Infinite precision](https://en.wikipedia.org/wiki/Arbitrary-precision_arithmetic) for basic
  arithmetic operations (addition, subtraction, multiplication, division) so that calculations
  never lose precision.

## Getting started
Prerequisites:
- Your computer must be running Windows 11, build 22000 or newer.
- Install the latest version of [Visual Studio](https://developer.microsoft.com/en-us/windows/downloads) (the free community edition is sufficient).
  - Install the "Universal Windows Platform Development" workload.
  - Install the optional "C++ Universal Windows Platform tools" component.
  - Install the latest Windows 11 SDK.
  - Histórico de cálculos e recursos de memória.
- Conversão entre várias unidades de medida.
- Conversão de moeda com base nos dados recuperados do [Bing](https://www.bing.com).
- [Precisão infinita](https://en.wikipedia.org/wiki/Arbitrary-precision_arithmetic) para
  operações aritméticas (adição, subtração, multiplicação, divisão) para que os cálculos
  nunca perca a precisão.

  ![Visual Studio Installation Screenshot](docs/Images/VSInstallationScreenshot.png)
- Install the [XAML Styler](https://marketplace.visualstudio.com/items?itemName=TeamXavalon.XAMLStyler) Visual Studio extension.

- Get the code:
    ```
    git clone https://github.com/Microsoft/calculator.git
    ```

- Open [src\Calculator.sln](/src/Calculator.sln) in Visual Studio to build and run the Calculator app.
- For a general description of the Calculator project architecture see [ApplicationArchitecture.md](docs/ApplicationArchitecture.md).
- To run the UI Tests, you need to make sure that
  [Windows Application Driver (WinAppDriver)](https://github.com/microsoft/WinAppDriver/releases/latest)
  is installed.

## Contributing
Want to contribute? The team encourages community feedback and contributions. Please follow our [contributing guidelines](CONTRIBUTING.md).

If Calculator is not working properly, please file a report in the [Feedback Hub](https://insider.windows.com/en-us/fb/?contextid=130).
We also welcome [issues submitted on GitHub](https://github.com/Microsoft/calculator/issues).

## Roadmap
For information regarding Windows Calculator plans and release schedule, please see the [Windows Calculator Roadmap](docs/Roadmap.md).

### Graphing Mode
Adding graphing calculator functionality [is on the project roadmap](https://github.com/Microsoft/calculator/issues/338) and we hope that this project can create a great end-user experience around graphing. To that end, the UI from the official in-box Windows Calculator is currently part of this repository, although the proprietary Microsoft-built graphing engine, which also drives graphing in Microsoft Mathematics and OneNote, is not. Community members can still be involved in the creation of the UI, however developer builds will not have graphing functionality due to the use of a [mock implementation of the engine](/src/GraphingImpl/Mocks) built on top of a
[common graphing API](/src/GraphingInterfaces).

## Diagnostic Data
This project collects usage data and sends it to Microsoft to help improve our products and services.
Read our [privacy statement](https://go.microsoft.com/fwlink/?LinkId=521839) to learn more.
Diagnostic data is disabled in development builds by default, and can be enabled with the `SEND_DIAGNOSTICS`
build flag.

## Currency Converter
Windows Calculator includes a currency converter feature that uses mock data in developer builds. The data that
Microsoft uses for the currency converter feature (e.g., in the retail version of the application) is not licensed
for your use. The mock data will be clearly identifiable as it references planets instead of countries,
and remains static regardless of selected inputs.

Instale a extensão do Visual Studio [XAML Styler](https://marketplace.visualstudio.com/items?itemName=TeamXavalon.XAMLStyler).

- Obtenha o código:
    ```
    git clone https://github.com/Microsoft/calculator.git
    ```

- Abra [src\Calculator.sln](/src/Calculator.sln) no Visual Studio para criar e executar o aplicativo Calculadora.
- Para obter uma descrição geral da arquitetura do projeto Calculadora, consulte [ApplicationArchitecture.md](docs/ApplicationArchitecture.md).
- Para executar os testes de interface do usuário, você precisa se certificar de que
  [Driver de aplicativo do Windows (WinAppDriver)](https://github.com/microsoft/WinAppDriver/releases/latest)
  está instalado.

## Contribuindo
Quer contribuir? A equipe incentiva o feedback e as contribuições da comunidade. Siga nossas [diretrizes de contribuição](CONTRIBUTING.md).

Se a Calculadora não estiver funcionando corretamente, envie um relatório no [Hub de comentários](https://insider.windows.com/en-us/fb/?contextid=130).
Também aceitamos [problemas enviados no GitHub](https://github.com/Microsoft/calculator/issues).

## Roteiro
Para obter informações sobre planos e cronograma de lançamento da Calculadora do Windows, consulte o [Roteiro da Calculadora do Windows](docs/Roadmap.md).

### Modo Gráfico
Adicionar a funcionalidade da calculadora gráfica [está no roteiro do projeto](https://github.com/Microsoft/calculator/issues/338) e esperamos que este projeto possa criar uma ótima experiência do usuário final em relação aos gráficos. Para esse fim, a interface do usuário da calculadora oficial do Windows atualmente faz parte desse repositório, embora o mecanismo gráfico proprietário da Microsoft, que também conduz gráficos no Microsoft Mathematics e no OneNote, não seja. Os membros da comunidade ainda podem estar envolvidos na criação da interface do usuário, no entanto, as compilações do desenvolvedor não terão funcionalidade gráfica devido ao uso de uma [implementação simulada do mecanismo](/src/GraphingImpl/Mocks) construída sobre uma
[API gráfica comum](/src/GraphingInterfaces).

## Dados de diagnóstico
Este projeto coleta dados de uso e os envia à Microsoft para ajudar a melhorar nossos produtos e serviços.
Leia nossa [declaração de privacidade](https://go.microsoft.com/fwlink/?LinkId=521839) para saber mais.
Os dados de diagnóstico são desativados nas compilações de desenvolvimento por padrão e podem ser ativados com o `SEND_DIAGNOSTICS`
construir bandeira.

## Conversor de moeda
A Calculadora do Windows inclui um recurso de conversão de moeda que usa dados fictícios em compilações de desenvolvedores. os dados que
Microsoft usa para o recurso de conversão de moeda (por exemplo, na versão de varejo do aplicativo) não está licenciado
para seu uso. Os dados simulados serão claramente identificáveis, pois fazem referência a planetas em vez de países,
e permanece estático independentemente das entradas selecionadas.

## Reporting Security Issues
Please refer to [SECURITY.md](./SECURITY.md).

## License
Copyright (c) Microsoft Corporation. All rights reserved.

Licensed under the [MIT License](./LICENSE).
## Relatando problemas de segurança
Consulte [SECURITY.md](./SECURITY.md).

## Licença
Direitos autorais (c) Microsoft Corporation. Todos os direitos reservados.

Licenciado sob a [Licença MIT](./LICENSE).
