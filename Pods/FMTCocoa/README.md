# FMTCocoa

[![License](http://img.shields.io/:license-apache-orange.svg)](http://www.apache.org/licenses/LICENSE-2.0) 

!NOTE!
For public release, change podspec file `git@github.com:` to use HTTPS.

## Quickstart

demo

## Summary

This repo contains a [CocoaPod](https://cocoapods.org/) wrapper for the c++ 
[fmt](https://github.com/fmtlib/fmt) project, which provides a popular 
initial implemenation of `std::format` for projects that target C++ specs
prior to C++20. (At the time of writing, widespread compiler support of
C++20 is still being rolled out).

For most flexible building, this CocoaPod forces the header-only version
of `fmt`.

Note that the license used in this repo is independent of that used in `fmt`;
`fmt` currently has a very permissive [MIT-like license](https://github.com/fmtlib/fmt/blob/master/LICENSE.rst).


## Development

### Tags
We keep tags in this repo in sync with [`fmt` releases](https://github.com/fmtlib/fmt/releases).

To create a new release:
 * Clone this repo including submodules (`git clone --recursive`)
 * `cd fmt_root` and `git checkout` the release of `fmt` that
      you want.
 * Edit the podspec in this repo to match the version tag you checked out.
 * Create a PR and merge your changes to this repo.
 * Push a tag on *this repo* with a name that matches the version name
      used in your updated podspec (which should also match a `fmt`
      tag).

