// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Net;
using System.Net.Http;

namespace CalculatorUITestFramework
{
    public class WinAppDriverLocalServer : IDisposable
    {
        private const int WinAppDriverDefaultPort = 4723;
        private static readonly IPAddress WinAppDriverDefaultIp = IPAddress.Loopback;

        private readonly Process winAppDriverProcess;
        private bool processExited = false;

        public Uri ServiceUrl => new UriBuilder()
        {
            Scheme = "http",
            Host = WinAppDriverDefaultIp.ToString(),
            Port = WinAppDriverDefaultPort,
        }.Uri;

        public WinAppDriverLocalServer()
        {
            var driverRelativePath = @"Windows Application Driver\winappdriver.exe";
            string path = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.ProgramFiles), driverRelativePath);
            if (!File.Exists(path))
            {
                path = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.ProgramFilesX86), driverRelativePath);
            }

            if (!File.Exists(path))
            {
                throw new Exception("Could not find winappdriver.exe in program files. Make sure WinAppDriver is installed: https://aka.ms/winappdriver");
            }

            winAppDriverProcess = new Process();
            winAppDriverProcess.StartInfo.CreateNoWindow = true;
            winAppDriverProcess.StartInfo.FileName = path;
            winAppDriverProcess.StartInfo.UseShellExecute = false;
            winAppDriverProcess.StartInfo.RedirectStandardInput = true;
            winAppDriverProcess.StartInfo.RedirectStandardOutput = true;
            winAppDriverProcess.StartInfo.RedirectStandardError = true;
            winAppDriverProcess.OutputDataReceived += OnProcessOutputDataReceived;
            winAppDriverProcess.ErrorDataReceived += OnProcessErrorDataReceived;
            if (!winAppDriverProcess.Start())
            {
                throw new Exception("WinAppDriver process failed to start.");
            }
            winAppDriverProcess.BeginOutputReadLine();
            winAppDriverProcess.BeginErrorReadLine();

            var pingUri = new Uri(ServiceUrl, "/status");
            var status = new HttpClient().GetAsync(pingUri);
            var timeout = TimeSpan.FromSeconds(10);
            if (!status.Wait(timeout))
            {
                throw new Exception($"Request to WinAppDriver at {pingUri} timed out after {timeout.TotalSeconds} seconds.");
            }

            status.Result.EnsureSuccessStatusCode();
        }

        private void OnProcessOutputDataReceived(object sender, DataReceivedEventArgs e)
        {
            var data = e.Data?.Replace("\0", string.Empty);
            if (!string.IsNullOrEmpty(data))
            {
                Console.WriteLine(PrependWinAppDriverToEachLine(data));
            }
        }

        private void OnProcessErrorDataReceived(object sender, DataReceivedEventArgs e)
        {
            var data = e.Data?.Replace("\0", string.Empty);
            if (!string.IsNullOrEmpty(data))
            {
                Console.Error.WriteLine(PrependWinAppDriverToEachLine(data));
            }
        }

        private string PrependWinAppDriverToEachLine(string data)
        {
            return string.Join("\r\n", data
                .ReplaceLineEndings("\n")
                .Split("\n")
                .Select(line => "WinAppDriver> " + line));
        }

        ~WinAppDriverLocalServer()
        {
            if (!processExited)
            {
                winAppDriverProcess.Kill();
                winAppDriverProcess.WaitForExit();
            }
        }

        public void Dispose()
        {
            winAppDriverProcess.Kill();
            winAppDriverProcess.WaitForExit();
            processExited = true;
            GC.SuppressFinalize(this);
        }
    }
}
