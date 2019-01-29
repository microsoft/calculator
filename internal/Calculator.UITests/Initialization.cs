// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.IO;
using System.Runtime.InteropServices;
using Calculator.UIAutomationLibrary;
using Microsoft.OneCoreUap.Test.AppModel;
using WEX.TestExecution;
using WEX.TestExecution.Markup;

namespace Calculator.UITests
{
    [TestClass]
    public class Initialization
    {
        [DllImport("AppModel.TestHelper.dll")]
        private static extern Int32 WinRTHelper_Register();

        [AssemblyInitialize]
        [TestProperty("CoreClrProfile", "TestNetv2.0")]
        [TestProperty("RunFixtureAs:Assembly", "System")]
        public static void AssemblySetup(TestContext context)
        {
            Verify.AreEqual(0, WinRTHelper_Register());
            TestHelper.Initialize();

            // Install and Start the Etw.Service service to enable the use of EtwWaiter.
            EtwHelper.InstallAndStartETWService();

            bool installApp = false;
            if (context.Properties.Contains("InstallApp") && (bool.TryParse(context.Properties["InstallApp"].ToString(), out installApp)) && installApp)
            {
                string certToDeploy = Path.Combine(TAEFHelper.GetTestDeploymentDirectory(), Constants.CertificateFileName);
                InstallHelper.InstallCertFile(certToDeploy);

                string vcLibsToDeploy = Path.Combine(TAEFHelper.GetTestDeploymentDirectory(), Constants.VCLibsPackageFileName);
                string winUIToDeploy = Path.Combine(TAEFHelper.GetTestDeploymentDirectory(), Constants.WinUIPackageFileName);
                string appxToDeploy = Path.Combine(TAEFHelper.GetTestDeploymentDirectory(), Constants.PackageFileName);
                Impersonater.RunAs(Impersonater.RunAsUser.RestrictedUser, () => InstallHelper.InstallPackage(appxToDeploy, vcLibsToDeploy, winUIToDeploy));
            }
        }

        [AssemblyCleanup]
        [TestProperty("RunFixtureAs:Assembly", "System")]
        public static void AssemblyCleanup()
        {
            // Stop and remove the Etw.Service service.
            EtwHelper.StopAndRemoveETWService();

            TestHelper.Uninitialize();
        }
    }
}
