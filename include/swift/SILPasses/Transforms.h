//===-- Transforms.h - Swift Transformations  -------------------*- C++ -*-===//
//
// This source file is part of the Swift.org open source project
//
// Copyright (c) 2014 - 2015 Apple Inc. and the Swift project authors
// Licensed under Apache License v2.0 with Runtime Library Exception
//
// See http://swift.org/LICENSE.txt for license information
// See http://swift.org/CONTRIBUTORS.txt for the list of Swift project authors
//
//===----------------------------------------------------------------------===//
#ifndef SWIFT_SILPASSES_TRANSFORMS_H
#define SWIFT_SILPASSES_TRANSFORMS_H

#include "swift/SILPasses/PassManager.h"

namespace swift {
  class SILModule;
  class SILFunction;

  /// The base class for all SIL-level transformations.
  class SILTransform {
  public:
    /// The kind of transformation passes we use.
    enum class TransformKind {
      Function,
      Module,
    };

    /// Stores the kind of derived class.
    const TransformKind Kind;

  protected:
    // The pass manager that runs this pass.
    SILPassManager* PM;

  public:
    /// C'tor. \p K indicates the kind of derived class.
    SILTransform(SILTransform::TransformKind K) : Kind(K), PM(0) {}

    /// D'tor.
    virtual ~SILTransform() = default;

    /// Returns the kind of derived class.
    TransformKind getKind() const { return Kind; }

    /// Inject the pass manager running this pass.
    void injectPassManager(SILPassManager *PMM) { PM = PMM; }

  protected:
    /// \brief Searches for an analysis of type T in the list of registered
    /// analysis. If the analysis is not found, the program terminates.
    template<typename T>
    T* getAnalysis() { return PM->getAnalysis<T>(); }

    SILOptions &getOptions() { return PM->getOptions(); }
  };

  /// A transformation that operates on functions.
  class SILFunctionTransform : public SILTransform {
    SILFunction *F;

  public:
    /// C'tor.
    SILFunctionTransform() : SILTransform(TransformKind::Function), F(0) {}

    /// The entry point to the transformation.
    virtual void run() = 0;

    static bool classof(const SILTransform *S) {
      return S->getKind() == TransformKind::Function;
    }

    void injectFunction(SILFunction *Func) { F = Func; }

  protected:
    SILFunction *getFunction() { return F; };

    void invalidateAnalysis(SILAnalysis::InvalidationKind K) {
      PM->invalidateAnalysis(F, K);
    }
  };

  /// A transformation that operates on modules.
  class SILModuleTransform : public SILTransform {
    SILModule *M;

  public:
    /// C'tor.
    SILModuleTransform() : SILTransform(TransformKind::Module), M(0) {}

    /// The entry point to the transformation.
    virtual void run() = 0;

    static bool classof(const SILTransform *S) {
      return S->getKind() == TransformKind::Module;
    }

    void injectModule(SILModule *Mod) { M = Mod; }

  protected:
    SILModule *getModule() { return M; };

    void invalidateAnalysis(SILAnalysis::InvalidationKind K) {
      PM->invalidateAnalysis(K);
    }
  };

} // end namespace swift

#endif

