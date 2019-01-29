// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.IO;
using System.Linq;
using System.Security.Cryptography.X509Certificates;
using System.Threading;
using Microsoft.OneCoreUap.Test.AppModel;
using WEX.Logging.Interop;
using Windows.Foundation;
using Windows.Management.Deployment;

namespace Calculator.UIAutomationLibrary
{
    public class InstallHelper
    {
        public static void InstallCertFile(string certFilePath)
        {
            // Ensure cert exists.
            if (!File.Exists(certFilePath))
            {
                throw new FileNotFoundException(certFilePath);
            }

            // For some reason, attempting to use CertificateHelper.InstallFromSignedPackage() to install
            // the certificate associated with the appx package fails with the error:
            //     "A certificate chain could not be built to a trusted root authority."
            // The reason is that the cert needs to be installed in 'StoreName.TrustedPeople',
            // but DeploymentHelper.AddPackage() attempts to install it in 'StoreName.Root'.
            // Therefore, the cert has been installed manually beforehand.
            Log.Comment($"Starting install of certificate at {certFilePath}");

            X509Certificate2 certificate = new X509Certificate2(certFilePath);
            X509Store trustedPeopleStore = new X509Store(StoreName.TrustedPeople, StoreLocation.LocalMachine);
            trustedPeopleStore.Open(OpenFlags.MaxAllowed);
            trustedPeopleStore.Add(certificate);

            Log.Comment($"Completed install of certificate");
        }

        /// <summary>
        /// Upgrades the appx/appxbundle from the path if needed.
        /// </summary>
        public static void InstallPackage(string appxFilePath, params string[] dependencyAppxFilePaths)
        {
            // Ensure the files exist.
            if (!File.Exists(appxFilePath))
            {
                throw new FileNotFoundException(appxFilePath);
            }
            foreach (var path in dependencyAppxFilePaths.Where(p => !File.Exists(p)))
            {
                throw new FileNotFoundException(path);
            }

            Log.Comment($"Starting install of app package at {appxFilePath}");

            PackageManager packageManager = new PackageManager();
            var deploymentOperation = packageManager.AddPackageAsync(
                new Uri(appxFilePath),
                dependencyAppxFilePaths.Select(d => new Uri(d)),
                DeploymentOptions.ForceApplicationShutdown | DeploymentOptions.ForceTargetApplicationShutdown | DeploymentOptions.ForceUpdateFromAnyVersion);

            WaitForDeploymentOperation(deploymentOperation);

            Log.Comment("Completed install of app package");
        }

        /// <summary>
        /// Waits for a deployment operation to complete
        /// </summary>
        private static void WaitForDeploymentOperation(IAsyncOperationWithProgress<DeploymentResult, DeploymentProgress> operation)
        {
            ManualResetEvent isCompletedEvent = new ManualResetEvent(false);
            operation.Completed = (IAsyncOperationWithProgress<DeploymentResult, DeploymentProgress> asyncInfo, AsyncStatus asyncStatus) =>
            {
                isCompletedEvent.Set();
            };

            isCompletedEvent.WaitOne();
        }
    }
}
