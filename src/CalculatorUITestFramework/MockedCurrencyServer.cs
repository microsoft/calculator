using System;
using System.IO;
using System.Net;
using System.Web;

namespace CalculatorUITestFramework
{
    public class MockedCurrencyServer : IDisposable
    {
        private HttpListener _listener;

        public MockedCurrencyServer()
        {
            _listener = new HttpListener
            {
                Prefixes =
                {
                    // to trust below URL on your dev machine, run below command in cmd as admin
                    // netsh http add urlacl url="http://127.0.0.1:80/calctesting/file" user=everyone
                    "http://127.0.0.1/calctesting/file/",
                }
            };
            _listener.Start();
            _listener.BeginGetContext(OnGetContext, _listener);
            Console.WriteLine("MockedCurrencyServer started.");
        }

        ~MockedCurrencyServer() { DisposeImpl(); }

        public void Dispose()
        {
            DisposeImpl();
            GC.SuppressFinalize(this);
        }

        private void DisposeImpl()
        {
            _listener.Stop();
        }

        private static void OnGetContext(IAsyncResult result)
        {
            var listener = (HttpListener)result.AsyncState;
            HttpListenerContext context;
            try
            {
                context = listener.EndGetContext(result);
            }
            catch (HttpListenerException)
            {
                return;
            }
            Console.WriteLine($"MockedCurrencyServer: on request, url = {context.Request.Url}");
            var request = context.Request;
            var response = context.Response;
            if (request.HttpMethod == "GET" && TryGetServerFile(request.Url, out var content))
            {
                response.Headers.Add("Accept-Ranges", "bytes");
                response.Headers.Add("Content-Type", "application/json");
                var data = System.Text.Encoding.UTF8.GetBytes(content);
                response.StatusCode = 200;
                response.ContentLength64 = data.Length;
                response.OutputStream.Write(data, 0, data.Length);
                response.OutputStream.Close();
                response.Close();
                Console.WriteLine($"MockedCurrencyServer: on request, completed.");
            }
            else
            {
                response.StatusCode = 400;
                response.OutputStream.Close();
            }
            listener.BeginGetContext(OnGetContext, listener);
        }

        private static bool TryGetServerFile(Uri uri, out string content)
        {
            var query = HttpUtility.ParseQueryString(uri.Query);
            var name = query["id"];
            switch (name)
            {
                case "currency converter data":
                    content = File.ReadAllText("MockCurrencyConverterData.json");
                    return true;
                case "currency static data":
                    content = File.ReadAllText("MockCurrencyStaticData.json");
                    return true;
                default:
                    content = null;
                    return false;
            }
        }
    }
}
