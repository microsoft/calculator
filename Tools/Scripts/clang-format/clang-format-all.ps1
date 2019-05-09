<#
    .DESCRIPTION
        Helper script to format all header and source files in the repository.

        By default, the script will recursively search under the repo root for
        files to format.  Users can give explicit parameters indicating how the
        search should include and exclude filetypes.

        If users don't want the search functionality, they can instead provide
        an explicit list of files to format.

    .PARAMETER RepoRoot
        Full path to the root of the repository which is the target of the search.
        Will default to the root of the current working directory.

    .PARAMETER Include
        Array of filetype extensions to target for formatting.
        By default, targets standard extensions for header and source files.
        Follows the same rules as the -Include parameter for Get-ChildItem.

    .PARAMETER Exclude
        Array of filetype extensions to exclude from formatting.
        By default, excludes generated XAML files.
        Follows the same rules as the -Exclude paramter for Get-ChildItem.

    .PARAMETER Files
        Array of files to format. The script will exit if one of the provided
        filepaths does not exist.

    .EXAMPLE
        .\clang-format-all.ps1

        Formats all header and source files under the repository root.

    .EXAMPLE
        .\clang-format-all.ps1 -RepoRoot 'S:\repos\calculator' -Include '*.h', '*.cpp' -Exclude '*.g.*'

        Formats all *.h and *.cpp files under 'S:\repos\calculator', excluding files with an extension
        like *.g.*

    .EXAMPLE
        .\clang-format-all.ps1 -File 'S:\repos\calculator\src\CalcViewModel\UnitConverterViewModel.h', 'S:\repos\calculator\src\CalcViewModel\MemoryItemViewModel.cpp'

        Formats the specified files.
#>
[CmdletBinding( DefaultParameterSetName = 'Search' )]
param(
    [Parameter( ParameterSetName = 'Search' )]
    [ValidateScript({ Test-Path -PathType Container -Path $_ })]
    [string] $RepoRoot = "$( git rev-parse --show-toplevel )",

    [Parameter( ParameterSetName = 'Search' )]
    [string[]] $Include = ( '*.h', '*.hh', '*.hpp', '*.c', '*.cc', '*.cpp' ),

    [Parameter( ParameterSetName = 'Search' )]
    [string[]] $Exclude = '*.g.*',

    [Parameter(
        ParameterSetName = 'Explicit',
        Mandatory)]
    [ValidateScript({
        $_ | Where-Object { -not (Test-Path -PathType Leaf -Path $_) } |
             ForEach-Object { throw "Could not find file: [$_]" }

        return $true
    })]
    [string[]] $Files
)

if ($PSCmdlet.ParameterSetName -eq 'Explicit')
{
    # Use the file paths we were given.
    $targetFiles = @($Files)
}
else
{
    # Gather the files to be formatted.
    $targetFiles = @(
        Get-ChildItem -Recurse -Path $RepoRoot -Include $Include -Exclude $Exclude |
            Select-Object -ExpandProperty FullName
    )
}

# Format the files.
$formatParams = @(
    '-i'          # In-place
    '-style=file' # Search for a .clang-format file in the parent directory of the source file.
    '-verbose'
)

clang-format $formatParams $targetFiles