#requires -RunAsAdministrator
param(
    [Parameter(Position = 0, Mandatory = $true)][string]$AppToSign,
    [string]$SignTool = "C:\Program Files (x86)\Windows Kits\10\bin\10.*\x64\signtool.exe"
)

$AppToSign = (Resolve-Path -Path $AppToSign)[-1]
Write-Host "AppToSign: $AppToSign"
$SignTool = (Resolve-Path -Path $SignTool)[-1]
Write-Host "SignTool: $SignTool"
if ((Test-Path -Path $SignTool -PathType Leaf) -ne $true) {
    Write-Error "signtool is not found with the given argument: $SignTool" -ErrorAction Stop
}

$codeSignOid = New-Object -TypeName "System.Security.Cryptography.Oid" -ArgumentList @("1.3.6.1.5.5.7.3.3")
$oidColl = New-Object -TypeName "System.Security.Cryptography.OidCollection"
$oidColl.Add($codeSignOid) > $null
$publisher = "CN=Microsoft Corporation, O=Microsoft Corporation, L=Redmond, S=Washington, C=US"
$certReq = New-Object -TypeName "System.Security.Cryptography.X509Certificates.CertificateRequest" `
    -ArgumentList @($publisher, ([System.Security.Cryptography.ECDsa]::Create()), "SHA256") 
$certReq.CertificateExtensions.Add((New-Object -TypeName "System.Security.Cryptography.X509Certificates.X509EnhancedKeyUsageExtension" `
            -ArgumentList @($oidColl, $false)))
$now = Get-Date
$cert = $certReq.CreateSelfSigned($now, $now.AddHours(1))

$pfxFile = "$($env:TEMP)\$(New-Guid).pfx"
[System.IO.File]::WriteAllBytes($pfxFile, $cert.Export("Pfx"))
Write-Host "Exported PFX: $pfxFile"

& $SignTool sign /fd SHA256 /a /f $pfxFile $AppToSign
Write-Host "Certificate Thumbprint: $($cert.Thumbprint.ToLower())"

Import-PfxCertificate -CertStoreLocation 'Cert:\LocalMachine\TrustedPeople' -FilePath $pfxFile > $null
