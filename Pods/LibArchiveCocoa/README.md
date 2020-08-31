# LibArchiveCocoa

[![License](http://img.shields.io/:license-apache-orange.svg)](http://www.apache.org/licenses/LICENSE-2.0) 

!NOTE!
For public release, change podspec file `git@github.com:` to use HTTPS.

## Quickstart

Simply include this pod in your `Podfile`:
```
pod 'LibArchiveCocoa', '~> 3.4.2'
```

## Summary

This repo contains a [CocoaPod](https://cocoapods.org/) wrapper for the 
[LibArchive](https://www.libarchive.org/) project, which provides a popular 
cross-platform open source solution for reading and writing Zip, Tar, and 
other archives.

Note that `LibArchive` is actually
[baked into iOS internally](https://opensource.apple.com/tarballs/libarchive/)
but concealed as a private API, so for practical use in your App you need to
build `LibArchive` and statically link it yourself.

`LibArchive` can *almost* be baked into an iOS app as-is from source, with
the exception that the user must provide a `config.h` platform header to
enable/disable certain features.  *This project provides that header,
as well as a `Podspec` for easy inclusion of `LibArchive` in your App.*
To reproduce the included `config.h`, start by cloning `LibArchive` locally 
and [build using `cmake -G Xcode -DENABLE_ICONV=OFF .`](https://github.com/libarchive/libarchive/wiki/BuildInstructions#MacOSXcode)
as documented on the `LibArchive` wiki.  You may need to make a few tweaks
to get the resulting `config.h` to build for the iOS simulator & device.

Note that the license used in this repo is independent of that used in `LibArchive`;
`LibArchive` currently has a [BSD-like license](https://github.com/libarchive/libarchive/blob/master/COPYING)


## Development

### Tags
We keep tags in this repo in sync with [LibArchive releases](https://github.com/libarchive/libarchive/releases).

To create a new release:
 * Clone this repo including submodules (`git clone --recursive`)
 * `cd libarchive` and `git checkout` the release of `LibArchive` that
      you want.
 * Edit the podspec in this repo to match the version tag you checked out.
 * Create a PR and merge your changes to this repo.
 * Push a tag on *this repo* with a name that matches the version name
      used in your updated podspec (which should also match a `LibArchive`
	  tag).

### Pushing the Podspec

When pushing the podspec, use `--allow-warnings` so we can ignore some irrelevant warnings:
```
$ pod repo push MyRepo LibArchiveCocoa.podspec.json --allow-warnings --use-libraries
```

