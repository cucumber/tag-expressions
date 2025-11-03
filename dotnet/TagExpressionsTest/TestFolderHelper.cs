using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Cucumber.TagExpressionsTest;

internal static class TestFolderHelper
{
    public static string TestFolder
    {
        get
        {
            var assemblyLocation = System.Reflection.Assembly.GetExecutingAssembly().Location;
            var testFolder = PathHelper.FindSiblingFolder(assemblyLocation, "testdata");
            if (testFolder == null)
            {
                throw new InvalidOperationException("Could not find 'testdata' folder relative to assembly location.");
            }
            return testFolder;
        }
    }
}

internal static class PathHelper
{
    /// <summary>
    /// Finds a sibling folder relative to the assembly location by moving upward
    /// through parent directories if the sibling does not exist at the initial level.
    /// </summary>
    /// <param name="assemblyLocation">The full path of the assembly (file or folder).</param>
    /// <param name="siblingFolderName">The name of the sibling folder to find.</param>
    /// <returns>The full path to the sibling folder if found; otherwise, null.</returns>
    public static string? FindSiblingFolder(string assemblyLocation, string siblingFolderName)
    {
        // Start from the directory of the assembly location
        var directory = Path.GetDirectoryName(assemblyLocation);
        if (directory == null)
            return null;

        var currentDir = new DirectoryInfo(directory);

        while (currentDir.Parent != null)
        {
            // Construct the sibling folder path at this level
            string siblingPath = Path.Combine(currentDir.Parent.FullName, siblingFolderName);

            if (Directory.Exists(siblingPath))
            {
                return siblingPath; // Found the sibling folder
            }

            // Move one directory up to continue probing
            currentDir = currentDir.Parent;
        }

        // Reached root with no sibling folder found
        return null;
    }
}
