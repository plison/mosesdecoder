#ifndef moses_GlobalLexicalModel_h
#define moses_GlobalLexicalModel_h

#include <stdexcept>
#include <string>
#include <vector>
#include <memory>
#include "StatelessFeatureFunction.h"
#include "moses/Factor.h"
#include "moses/Phrase.h"
#include "moses/TypeDef.h"
#include "moses/Util.h"
#include "moses/WordsRange.h"
#include "moses/FactorTypeSet.h"
#include "moses/Sentence.h"

#ifdef WITH_THREADS
#include <boost/thread/tss.hpp>
#endif

namespace Moses
{

class Factor;
class Phrase;
class Hypothesis;
class InputType;

/** Discriminatively trained global lexicon model
 * This is a implementation of Mauser et al., 2009's model that predicts
 * each output word from _all_ the input words. The intuition behind this
 * feature is that it uses context words for disambiguation
 */
class GlobalLexicalModel : public StatelessFeatureFunction
{
  typedef std::map< const Word*, std::map< const Word*, float, WordComparer >, WordComparer > DoubleHash;
  typedef std::map< const Word*, float, WordComparer > SingleHash;
  typedef std::map< const TargetPhrase*, float > LexiconCache;

  struct ThreadLocalStorage
  {
    LexiconCache cache;
    const Sentence *input;
  };

private:
  DoubleHash m_hash;
#ifdef WITH_THREADS
  boost::thread_specific_ptr<ThreadLocalStorage> m_local;
#else
  std::auto_ptr<ThreadLocalStorage> m_local;
#endif
  Word *m_bias;

  FactorMask m_inputFactors;
  FactorMask m_outputFactors;

  void LoadData(const std::string &filePath,
                const std::vector< FactorType >& inFactors,
                const std::vector< FactorType >& outFactors);

  float ScorePhrase( const TargetPhrase& targetPhrase ) const;
  float GetFromCacheOrScorePhrase( const TargetPhrase& targetPhrase ) const;

public:
  GlobalLexicalModel(const std::string &line);
	virtual ~GlobalLexicalModel();

  void InitializeForInput( Sentence const& in );

  void Evaluate(const PhraseBasedFeatureContext& context,
  							ScoreComponentCollection* accumulator) const;


  void EvaluateChart(
    const ChartBasedFeatureContext& context,
    ScoreComponentCollection* accumulator) const
  {
    throw std::logic_error("GlobalLexicalModel not supported in chart decoder, yet");
  }

};

}
#endif
