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

            this.winAppDriverProcess = new Process();
            this.winAppDriverProcess.StartInfo.CreateNoWindow = true;
            this.winAppDriverProcess.StartInfo.FileName = path;
            this.winAppDriverProcess.StartInfo.UseShellExecute = false;
            this.winAppDriverProcess.StartInfo.RedirectStandardInput = true;
            this.winAppDriverProcess.StartInfo.RedirectStandardOutput = true;
            this.winAppDriverProcess.StartInfo.RedirectStandardError = true;
            this.winAppDriverProcess.OutputDataReceived += this.OnProcessOutputDataReceived;
            this.winAppDriverProcess.ErrorDataReceived += this.OnProcessErrorDataReceived;
            if (!this.winAppDriverProcess.Start())
            {
                throw new Exception("WinAppDriver process failed to start.");
            }
            this.winAppDriverProcess.BeginOutputReadLine();
            this.winAppDriverProcess.BeginErrorReadLine();

            var pingUri = new Uri(this.ServiceUrl, "/status");
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
                Console.WriteLine(this.PrependWinAppDriverToEachLine(data));
            }
        }

        private void OnProcessErrorDataReceived(object sender, DataReceivedEventArgs e)
        {
            var data = e.Data?.Replace("\0", string.Empty);
            if (!string.IsNullOrEmpty(data))
            {
                Console.Error.WriteLine(this.PrependWinAppDriverToEachLine(data));
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
            if (!this.processExited)
            {
                this.winAppDriverProcess.Kill();
                this.winAppDriverProcess.WaitForExit();
            }
        }

        public void Dispose()
        {
            this.winAppDriverProcess.Kill();
            this.winAppDriverProcess.WaitForExit();
            this.processExited = true;
            GC.SuppressFinalize(this);
        }
    }
}
