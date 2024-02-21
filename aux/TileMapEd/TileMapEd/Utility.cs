using System;
using System.IO;
using System.Text;
using System.Threading;
using System.Diagnostics;

namespace utility
{
	public class ExecutionResult
	{
		public string output,
			          error;
		public int    exit;

		public ExecutionResult(int exitCode, string standardOutput, string standardError)
		{
			exit   = exitCode;
			output = standardOutput;
            error  = standardError;
		}
	}

	/// <summary>
	/// Simple utility class for doing various things.
	/// </summary>
	public class Utility
	{
		/// <summary>
		/// 
		/// </summary>
		/// <param name="cmd"></param>
		/// <param name="output"></param>
		/// <returns></returns>
		public static ExecutionResult Run(string cmd)
		{
			StringWriter swStdOut = new StringWriter(), swStdErr = new StringWriter();
			int exit = Run(cmd, swStdOut, swStdErr);
			return new ExecutionResult(exit, swStdOut.GetStringBuilder().ToString(), swStdErr.GetStringBuilder().ToString());
		}

		public static int Run(string cmd, TextWriter wOut, TextWriter wErr)
		{
			Process p = new Process();
			p.StartInfo.UseShellExecute = false;
			p.StartInfo.RedirectStandardOutput = true;
			p.StartInfo.RedirectStandardError = true;
			p.StartInfo.FileName = "cmd.exe";
			p.StartInfo.Arguments = "/C " + cmd;
			p.StartInfo.CreateNoWindow = true;
			p.Start();
			while (!p.HasExited)
			{
  				string output = p.StandardOutput.ReadToEnd(),
    				   error  = p.StandardError.ReadToEnd();
				if (wOut != null && output.Length != 0) wOut.Write(output);
				if (wErr != null && error.Length  != 0) wErr.Write(error);
				Thread.Sleep(1);
			}
			return p.ExitCode;
		}

		/// <summary>
		/// Returns the relative path from the parent to the file.
		/// <example>GetRelativePath( "C:\Folder\File", "C:\Parent" ) returns "Folder\File"</example>
		/// </summary>
		public static string GetRelativePath( string file, string parent )
		{
			Debug.Assert( file.Length > 0 );
			Debug.Assert( parent.Length > 0 );

			// split the paths
			string[] filePath = Path.GetDirectoryName( file ).TrimEnd( '\\' ).Split( '\\' );
			string[] parentPath = Path.GetDirectoryName( parent ).TrimEnd( '\\' ).Split( '\\' );

			int index = 0;

			// skip the identical portions
			while( ( index < filePath.Length ) &&
				( index < parentPath.Length ) &&
				( filePath[ index ] == parentPath[ index ] ) )
			{
				index++;
			}

			if( ( index == 0 ) && filePath[ 0 ].EndsWith( ":" ) )
			{
				// the file is on a different drive than the parent, so just
				// early out and return the full path
				return( file );
			}

			string result = "";

			// need to back out from the parent until we get to an overlapping path
			for( int i = index; i < parentPath.Length; i++ )
			{
				result += "..\\";
			}

			// now traverse down to the file's path
			for( int i = index; i < filePath.Length; i++ )
			{
				result += filePath[ i ];

				result += "\\";
			}

			// add the filename
			result += Path.GetFileName( file );

			// if we got a relative path to the same location with no file, just default it
			if( result == "" )
			{
				result = ".\\";
			}

			return( result );
		}

		/// <summary>
		/// Returns the absolute path for the file given a relative path to another parent file.
		/// <example>GetAbsolutePath( "Folder\File", "C:\Parent" ) returns "C:\Folder\File"</example>
		/// </summary>
		public static string GetAbsolutePath( string file, string parent )
		{
			Debug.Assert( file.Length > 0 );
			Debug.Assert( parent.Length > 0 );

			// split the paths
			string[] filePath = Path.GetDirectoryName( file ).TrimEnd( '\\' ).Split( '\\' );
			string[] parentPath = Path.GetDirectoryName( parent ).TrimEnd( '\\' ).Split( '\\' );

			if( filePath[ 0 ].EndsWith( ":" ) )
			{
				// file is already an absolute path, so early out
				return( file );
			}

			int skipDirs = 0;

			// figure out how many folders to back out from the parent
			while( ( skipDirs < filePath.Length ) && ( filePath[ skipDirs ] == ".." ) )
			{
				skipDirs++;
			}

			string result = "";

			// build the parent part of the path
			for( int i = 0; i < parentPath.Length - skipDirs; i++ )
			{
				result += parentPath[ i ] + "\\";
			}

			// now add the file's part of the path
			for( int i = skipDirs; i < filePath.Length; i++ )
			{
				if( filePath[ i ] != "" )
				{
					result += filePath[ i ] + "\\";
				}
			}

			// add the filename
			result += Path.GetFileName( file );

			return( result );
		}

		/// <summary>
		/// Determines if the file at the given path exists and is read-only.
		/// </summary>
		/// <param name="filePath">File to look at.</param>
		/// <returns>True if the file exists and is read-only.</returns>
		public static bool IsReadOnly( string filePath )
		{
			if( File.Exists( filePath ) )
			{
				FileAttributes attributes = File.GetAttributes( filePath );

				if ((attributes & FileAttributes.ReadOnly) == FileAttributes.ReadOnly)
				{
					return true;
				}
			}

			return false;
		}

		public static string ReadString(string file)
		{
			FileStream stream = null;
			string     s      = null;
			try
			{
			    stream = File.OpenRead(file);
			    long sz = stream.Length;
			    byte[] data = new byte [sz];
			    stream.Read(data, 0, (int)sz);
				s = Encoding.ASCII.GetString(data);
			}
			finally
			{
				try { stream.Close(); } catch (Exception) {}
			}
			return s;
		}

		public static string Format(string sFormat, params object[] aValues)
		{
			StringBuilder s = new StringBuilder();
			s.AppendFormat(sFormat, aValues);
			return s.ToString();
		}
 	}
}